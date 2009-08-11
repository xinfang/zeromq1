/*
    Copyright (c) 2007-2009 FastMQ Inc.

    This file is part of 0MQ.

    0MQ is free software; you can redistribute it and/or modify it under
    the terms of the Lesser GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    0MQ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    Lesser GNU General Public License for more details.

    You should have received a copy of the Lesser GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <map>
#include <string>
using namespace std;

#include <zmq/platform.hpp>
#ifdef ZMQ_HAVE_WINDOWS
#else
#include <unistd.h>
#include <sys/socket.h>
#ifdef ZMQ_HAVE_OPENVMS
#include <sys/types.h>
#include <sys/time.h>
#else
#include <sys/select.h>
#endif

#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <poll.h>
#endif

#include <zmq.hpp>
#include <zmq/i_locator.hpp>
#include <zmq/fd.hpp>
#include <zmq/tcp_socket.hpp>
#include <zmq/tcp_listener.hpp>
#include <zmq/server_protocol.hpp>
#include <zmq/xmlParser.hpp>
using namespace zmq;

typedef map <string, attr_list_t> object_table_t;

//  Object repository.
static object_table_t objects;

static int register_object (tcp_socket_t *s)
{
    unsigned char size, reply;
    char buf [255];
    attr_list_t attrs;

    //  Read name.
    int n = s->read (&size, 1);
    if (n != 1)
        return -1;
    n = s->read (buf, size);
    if (n != size)
        return -1;
    string name = string (buf, n);

    n = s->read (&size, 1);
    if (n != 1)
        return -1;

    //  Read attributes.
    while (size > 0) {

        n = s->read (buf, size);
        if (n != size)
            return -1;
        string key = string (buf, n);

        n = s->read (&size, 1);
        if (n != 1)
            return -1;

        n = s->read (buf, size);
        if (n != size)
            return -1;
        string value = string (buf, n);

        attrs [key] = value;

        n = s->read (&size, 1);
        if (n != 1)
            return -1;
    }

    //  Signal success.
    reply = create_ok_id;
    n = s->write (&reply, 1);
    if (n != 1)
        return -1;

#ifdef ZMQ_TRACE
    printf("Object %s registered:\n", name.c_str ());
    for (attr_list_t::iterator it = attrs.begin (); it != attrs.end (); it ++)
        printf ("\t%s = %s\n", (*it).first.c_str (), (*it).second.c_str ());
#endif

    //  Register object.
    pair <object_table_t::iterator, bool> res =
        objects.insert (object_table_t::value_type (name, attrs));
    if (!res.second)
        res.first->second = attrs;

    return 0;
}

static int query_object (tcp_socket_t *s)
{
    unsigned char size, reply;
    char buf [255];

    //  Read name.
    int n = s->read (&size, 1);
    if (n != 1)
        return -1;
    n = s->read (buf, size);
    if (n != size)
        return -1;
    string name = string (buf, n);

    //  Loop up object in the registry.
    object_table_t::iterator obj = objects.find (name);
    if (obj == objects.end ()) {

        //  Signal error.
        reply = fail_id;
        n = s->write (&reply, 1);
        if (n != 1)
            return -1;

#ifdef ZMQ_TRACE
        printf ("Error when looking for an object: "
            "object %s not registered\n", name.c_str ());
#endif
        return 0;
    }

    attr_list_t &attrs = (*obj).second;

    //  Signal success.
    reply = get_ok_id;
    n = s->write (&reply, 1);
    if (n != 1)
        return -1;

    //  Write attributes.
    for (attr_list_t::iterator it = attrs.begin ();
            it != attrs.end (); it ++) {

        const string &key = (*it).first;
        const string &val = (*it).second;

        size = key.size ();
        n = s->write (&size, 1);
        if (n != 1)
            return -1;
        n = s->write (key.c_str (), size);
        if (n != size)
            return -1;

        size = val.size ();
        n = s->write (&size, 1);
        if (n != 1)
            return -1;
        n = s->write (val.c_str (), size);
        if (n != size)
            return -1;
    }

    //  Write terminator.
    size = 0;
    n = s->write (&size, 1);
    if (n != 1)
        return -1;

#ifdef ZMQ_TRACE
    printf ("Object %s retrieved:\n", name.c_str ());
    for (attr_list_t::iterator it = attrs.begin (); it != attrs.end (); it ++)
        printf ("\t%s = %s\n", (*it).first.c_str (), (*it).second.c_str ());
#endif

    return 0;
}

int main (int argc, char *argv [])
{
    uint16_t port = default_locator_port;
    string config_file = "";

    //  Parse the command line.
    int arg = 1;
    while (arg != argc) {
        if (string (argv [arg]) == "--help") {
            printf ("usage: zmq_server [--help] [--port <port-number>] "
                "[--config-file <filename>]\n");
            return 1;
        }
        else if (string (argv [arg]) == "--port") {
            assert (arg + 1 != argc);
            port = atoi (argv [arg + 1]);
            arg += 2;
        }
        else if (string (argv [arg]) == "--config-file") {
            assert (arg + 1 != argc);
            config_file = argv [arg + 1];
            arg += 2;
        }
    }

#ifdef ZMQ_HAVE_WINDOWS
    //  Initialise Windows socker layer.
    WORD version_requested = MAKEWORD (2, 2);
    WSADATA wsa_data;
    int rc = WSAStartup (version_requested, &wsa_data);
    errno_assert (rc == 0);
    assert (LOBYTE (wsa_data.wVersion) == 2 || HIBYTE (wsa_data.wVersion) == 2);
#endif

    //  Create a tcp_listener.
    char location [256];
    zmq_snprintf (location, sizeof (location), "0.0.0.0:%d", port);
    tcp_listener_t listening_socket (location, true);
     
    //	Create list of descriptors.
    typedef vector <tcp_socket_t*> socket_list_t;
    socket_list_t socket_list;

    //  Intitialise descriptors for select.
    fd_set result_set_fds, source_set_fds, error_set_fds;
    
    FD_ZERO (&source_set_fds);
    FD_ZERO (&result_set_fds);
    FD_ZERO (&error_set_fds); 
    fd_t fd_int = listening_socket.get_fd ();
    
    FD_SET (fd_int, &source_set_fds);
    fd_t maxfdp1 = fd_int + 1;
        

    if (!config_file.empty ()) {

        //  Load the configuration from a file.
        XMLNode root = XMLNode::parseFile (config_file.c_str ());
        assert (!root.isEmpty ());
        assert (strcmp (root.getName (), "root") == 0);

        //  Iteratate through all the 'node' subnodes.
        int n = 0;
        while (true) {
            XMLNode node = root.getChildNode ("node", n);
            if (node.isEmpty ())
                break;

            //  Fill in new node into locations map.
            const char *name = node.getAttribute ("name");
            assert (name);
            const char *location = node.getAttribute ("location");
            assert (location);
            attr_list_t attrs;
            attrs ["location"] = location;
            objects.insert (std::make_pair (name, attrs));
#ifdef ZMQ_TRACE
            printf ("Object %s created (%s).\n", name, location);
#endif

            n ++;
        }

    }

    while (true) {

       //  Select on the descriptors.
       int rc = 0;
       while (rc == 0) {
           
           memcpy (&result_set_fds, &source_set_fds, sizeof (source_set_fds));
           memcpy (&error_set_fds, &source_set_fds, sizeof (source_set_fds));
	            
           rc = select (maxfdp1, &result_set_fds, NULL, &error_set_fds, NULL);
#ifdef ZMQ_HAVE_WINDOWS
           win_assert (rc != SOCKET_ERROR);
#else
           errno_assert (rc != -1);
#endif
       }

        //  Accept incoming connection.
        if (FD_ISSET (fd_int, &result_set_fds)) {
#ifdef ZMQ_TRACE
            printf ("Opening connection.\n");
#endif
	    socket_list.push_back (new tcp_socket_t (listening_socket.accept (), 
                true));
            fd_t s = socket_list.back ()->get_fd ();
            FD_SET (s, &source_set_fds);
            
            if (maxfdp1 <= s)
                maxfdp1 = s + 1;

            continue;
        }
      
        //  Traverse all the sockets.
        for (socket_list_t::size_type pos = 0; pos < socket_list.size ();
              pos ++) {
 	   
            //  Get the socket being currently being processed.
            fd_t s = socket_list [pos]->get_fd ();

            //  If select signalises error, close the connection.
            if (FD_ISSET (s, &error_set_fds))
                goto error;

	    if (FD_ISSET (s, &result_set_fds)) {
           
                //  Read command ID.
                unsigned char cmd;

                int nbytes = socket_list [pos]->read (&cmd, 1);
                if (nbytes != 1)
                    goto error;

                //  Process individual commands.
                switch (cmd) {
                case create_id:
                    rc = register_object (socket_list [pos]);
                    break;
                case get_id:
                    rc = query_object (socket_list [pos]);
                    break;
                default:
                    goto error;
                }
            }

            //  If everything is OK, move to next socket.
            if (!rc)
                continue;
error:
#ifdef ZMQ_TRACE
            printf ("Closing connection.\n");
#endif
            //  Delete the tcp_socket from list of active connections. 
            delete socket_list [pos];
            socket_list.erase (socket_list.begin () + pos);

	    //  Erase the whole list of filedescriptors selectfds and add them
            //  back without the one erased from socket_list.
            FD_ZERO (&source_set_fds);
            FD_SET (fd_int , &source_set_fds);
            for (socket_list_t::size_type i = 0; i < socket_list.size (); i ++)                            
                FD_SET (socket_list [i]->get_fd (), &source_set_fds);
        }

#ifdef ZMQ_HAVE_OPENVMS
        //  Make OpenVMS compiler happy, otherwise would complain 
        //  about return 0 as an unreachable code
        if (false)
            break;
#endif

    }

#ifdef ZMQ_HAVE_WINDOWS

    //  Uninitialise Windows socket layer.
    rc = WSACleanup ();
    wsa_assert (rc != SOCKET_ERROR);
#endif
   
    return 0;

}


