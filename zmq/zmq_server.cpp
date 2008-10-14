/*
    Copyright (c) 2007-2008 FastMQ Inc.

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
#include "platform.hpp"

#ifdef ZMQ_HAVE_WINDOWS

#else
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <poll.h>
#endif
#include <string.h>
#include <vector>
#include <map>
#include <string>
using namespace std;

#include "config.hpp"
#include "stdint.hpp"
#include "err.hpp"
#include "zmq_server.hpp"
#include "tcp_socket.hpp"
using namespace zmq;

#ifdef ZMQ_HAVE_WINDOWS

//  Info about a single object.
struct object_info_t
{
    string interface_i;
    int fd;
};

//  Maps object name to object info.
typedef map <string, object_info_t> objects_t;

//  This function is used when there is socket disconnection. It cleans all
//  the objects registered by the connection.
void unregister (int s_, objects_t *objects_)
{
    for (int type_id = 0; type_id != type_id_count; type_id ++) {
        objects_t::iterator it = objects_[type_id].begin();
        while (it != objects_[type_id].end ()) {
            if (it->second.fd == s_) {
                objects_t::iterator tmp = it;
                it ++;
                objects_[type_id].erase (tmp);
                continue;
            }
            it ++;
        }
    }
}

int main (int argc, char *argv [])
{
    //  Check command line parameters.
    if ((argc != 1 && argc != 2) || (argc == 2 &&
          strcmp (argv [1], "--help") == 0)) {
              printf("argc: %d\n", argc);
        printf ("Usage: zmq_server [port]\n");
        printf ("Default port is %d.\n", (int) default_locator_port);
        
        return 1;
    }


    //  Create a tcp_listener.
    char interface_i [256] = "0.0.0.0:";
    char tmp [10];
    if (argc == 2 && (sizeof (argc)== 4 * sizeof (char) ||
        sizeof (argc)== 5 * sizeof (char) )) { 
	   //  The argument is port.
	   _snprintf_s (tmp, sizeof (argv [1]), "%s", argv [1]);
    }	
    else {
        sprintf_s (tmp, "%d", default_locator_port);   
    }
    strcat_s (interface_i, tmp);
 
    tcp_listener_t *listening_socket = new tcp_listener_t (interface_i);
     
    //	Create list of descriptors
    typedef vector <int> fd_list_t;
    fd_list_t fd_list;

    //  Intitialise descriptors for select
    fd_set result_set_fds, source_set_fds, error_set_fds;
    
    FD_ZERO (&source_set_fds);
    FD_ZERO (&result_set_fds);
    FD_ZERO (&error_set_fds); 
    int fd_int = listening_socket->get_fd ();
    FD_SET (fd_int, &source_set_fds);
    int maxfdp1 = fd_int + 1;
    
    fd_list.push_back (fd_int);
    
    //  Object repository. Individual object maps are placed into slots
    //  identified by the type ID of particular object.
    objects_t objects [type_id_count];
    
    while (true) {
        
	   //  Select on the descriptors.
       int rc = 0;
       while (rc == 0) {
            
           memcpy (&result_set_fds, &source_set_fds, sizeof (source_set_fds));
           memcpy (&error_set_fds, &source_set_fds, sizeof (source_set_fds));
	            
           rc = select (maxfdp1, &result_set_fds, NULL, &error_set_fds, NULL);
	       errno_assert (rc != -1);
       }    
      
       //  Traverse all the sockets.
       for (fd_list_t::size_type pos = 1; pos < fd_list.size (); pos ++) {

            //  Get the socket being currently being processed.
            int s = fd_list[pos];
            
            if (FD_ISSET (s, &error_set_fds)) {
               
                //  Unregister all the symbols registered by this connection
                //  and delete the descriptor from pollfds vector.
                unregister (s, objects);
                
                //  Erase the file descriptor from fd_list. 
                fd_list.erase (fd_list.begin () + pos);
                
                //  Erase the whole list of file descriptors selectfds and add
                //  them back without the one erased from fd_list.
                FD_ZERO (&source_set_fds);
                for (fd_list_t::size_type i = 0; i < fd_list.size (); i ++)
                    FD_SET (fd_list[i], &source_set_fds);
                 
                rc = closesocket (s);
                wsa_assert (rc != SOCKET_ERROR);
                continue;
            }
			else if (FD_ISSET (s, &result_set_fds)) {
           
                //  Read command ID.
                unsigned char cmd;
                unsigned char reply;
                int nbytes = recv (s, (char *) &cmd, 1, MSG_WAITALL);
                
                //  Connection closed by peer.
                if (nbytes == 0) {
           
                    //  Unregister all the symbols registered by this connection
                    //  and delete the descriptor from pollfds vector.
                    unregister (s, objects);
                    
                    //  Erase the filedescriptor from fd_list 
	                fd_list.erase (fd_list.begin () + pos);
	                
	                //  Erase the whole list of filedescriptors selectfds and add
	                //  them back without the one erased from fd_list.
	                FD_ZERO (&source_set_fds);
	                for (fd_list_t::size_type i = 0; i < fd_list.size (); i ++)
	                    FD_SET (fd_list[i], &source_set_fds);
                
                    rc = closesocket (s);
                    wsa_assert (rc != SOCKET_ERROR);
                    continue;
                }

                assert (nbytes == 1);

                switch (cmd) {
                case create_id:
                    {
                        
                        //  Parse type ID.
                        unsigned char type_id;
                        nbytes = recv (s, (char *) &type_id, 1, MSG_WAITALL);
                        assert (nbytes == 1);

                        //  Parse object name.
                        unsigned char size;
                        nbytes = recv (s, (char *) &size, 1, MSG_WAITALL);
                        assert (nbytes == 1);
                        char name [256];
                        nbytes = recv (s, name, size, MSG_WAITALL);
                        assert (nbytes == size);
                        name [size] = 0;

                        //  Parse interface_i.
                        nbytes = recv (s, (char *) &size, 1, MSG_WAITALL);
                        assert (nbytes == 1);
                        char interface_i [256];
                        nbytes = recv (s, interface_i, size, MSG_WAITALL);
                        assert (nbytes == size);
                        interface_i [size] = 0;

                        //  Insert object to the repository.
                        object_info_t info = {interface_i, s};
                        if (!objects [type_id].insert (
                              objects_t::value_type (name, info)).second) {

                            //  Send an error if the exchange already exists.
                            reply = fail_id;
                            nbytes = send (s, (char *) &reply, 1, 0);
                            assert (nbytes == 1);
#ifdef ZMQ_TRACE
                            printf ("Error when creating object: object %d:%s"
                                " already exists.\n", (int) type_id, name);
#endif
                            break;
                        }

                        //  Send reply command.
                        reply = create_ok_id;
                        nbytes = send (s, (char *) &reply, 1, 0);
                        assert (nbytes == 1);
#ifdef ZMQ_TRACE
                        printf ("Object %d:%s created (%s).\n", type_id, name,
                            interface_i);
#endif
                        break;
                    }
                case get_id:
                    {
                        //  Parse type ID.
                        unsigned char type_id;
                        nbytes = recv (s, (char *) &type_id, 1, MSG_WAITALL);
                        assert (nbytes == 1);

                        //  Parse object name.
                        unsigned char size;
                        nbytes = recv (s, (char *) &size, 1, MSG_WAITALL);
                        assert (nbytes == 1);
                        char name [256];
                        nbytes = recv (s, name, size, MSG_WAITALL);
                        assert (nbytes == size);
                        name [size] = 0;

                        //  Find the exchange in the repository.
                        objects_t::iterator it =
                            objects [type_id].find (name);
                        if (it == objects [type_id].end ()) {

                             //  Send the error.
                             reply = fail_id;
                             nbytes = send (s, (char *) &reply, 1, 0);
                             assert (nbytes == 1);
#ifdef ZMQ_TRACE
                             printf ("Error when looking for an object: "
                                 "object %d:%s does not exist.\n",
                                 (int) type_id, name);
#endif
                             break;
                        }

                        //  Send reply command.
                        reply = get_ok_id;
                        nbytes = send (s, (char *) &reply, 1, 0);
                        assert (nbytes == 1);

                        //  Send the interface_i.
                        size = it->second.interface_i.size ();
                        nbytes = send (s, (char *) &size, 1, 0);
                        assert (nbytes == 1);
                        nbytes = send (
                            s, it->second.interface_i.c_str (), size, 0);
                        assert (nbytes == size);

#ifdef ZMQ_TRACE
                        printf ("Object %d:%s retrieved (%s).\n", (int) type_id,
                            name, it->second.interface_i.c_str ());
#endif
                        break;
                    }
                default:
                    assert (false);
                }
            }

        }

        //  Accept incoming connection.
        if (FD_ISSET (fd_list [0], &result_set_fds)) {
            int s = listening_socket->accept ();
            errno_assert (s != -1);
            
            FD_SET (s, &source_set_fds);
            fd_list.push_back (s);
            if (maxfdp1 <= s)
                maxfdp1 = s + 1;
        }

    }

    return 0;
}

#else



//  Info about a single object.
struct object_info_t
{
    string interface_i;
    int fd;
};

//  Maps object name to object info.
typedef map <string, object_info_t> objects_t;

//  This function is used when there is socket disconnection. It cleans all
//  the objects registered by the connection.
void unregister (int s_, objects_t *objects_)
{
    for (int type_id = 0; type_id != type_id_count; type_id ++) {
        objects_t::iterator it = objects_[type_id].begin();
        while (it != objects_[type_id].end ()) {
            if (it->second.fd == s_) {
                objects_t::iterator tmp = it;
                it ++;
                objects_[type_id].erase (tmp);
                continue;
            }
            it ++;
        }
    }
}

int main (int argc, char *argv [])
{
    //  Check command line parameters.
    if ((argc != 1 && argc != 2) || (argc == 2 &&
          strcmp (argv [1], "--help") == 0)) {
        printf ("Usage: zmq_server [port]\n");
        printf ("Default port is %d.\n", (int) default_locator_port);
        return 1;
    }


    //  Create a tcp_listener.
    char interface_i [256] = "0.0.0.0:";
    char tmp [10];
    if (argc == 2 && (sizeof (argc)== 4 * sizeof (char) ||
        sizeof (argc)== 5 * sizeof (char) )) { 
	   //  The argument is port.
	   snprintf (tmp, sizeof (argv [1]), "%s", argv [1]);
    }	
    else {
        sprintf (tmp, "%d", default_locator_port);   
    }
    strcat (interface_i, tmp);
 
    tcp_listener_t *listening_socket = new tcp_listener_t (interface_i);
     
    //	Create list of descriptors
    typedef vector <int> fd_list_t;
    fd_list_t fd_list;

    //  Intitialise descriptors for select
    fd_set result_set_fds, source_set_fds, error_set_fds;
    
    FD_ZERO (&source_set_fds);
    FD_ZERO (&result_set_fds);
    FD_ZERO (&error_set_fds); 
    int fd_int = listening_socket->get_fd ();
    FD_SET (fd_int, &source_set_fds);
    int maxfdp1 = fd_int + 1;
    
    fd_list.push_back (fd_int);
    
    //  Object repository. Individual object maps are placed into slots
    //  identified by the type ID of particular object.
    objects_t objects [type_id_count];
    
    while (true) {
        
	   //  Select on the descriptors.
       int rc = 0;
       while (rc == 0) {
            
           memcpy (&result_set_fds, &source_set_fds, sizeof (source_set_fds));
           memcpy (&error_set_fds, &source_set_fds, sizeof (source_set_fds));
	            
           rc = select (maxfdp1, &result_set_fds, NULL, &error_set_fds, NULL);
	       errno_assert (rc != -1);
       }    
      
       //  Traverse all the sockets.
       for (fd_list_t::size_type pos = 1; pos < fd_list.size (); pos ++) {

            //  Get the socket being currently being processed.
            int s = fd_list[pos];
            
            if (FD_ISSET (s, &error_set_fds)) {
               
                //  Unregister all the symbols registered by this connection
                //  and delete the descriptor from pollfds vector.
                unregister (s, objects);
                
                //  Erase the file descriptor from fd_list. 
                fd_list.erase (fd_list.begin () + pos);
                
                //  Erase the whole list of file descriptors selectfds and add
                //  them back without the one erased from fd_list.
                FD_ZERO (&source_set_fds);
                for (fd_list_t::size_type i = 0; i < fd_list.size (); i ++)
                    FD_SET (fd_list[i], &source_set_fds);
                 
                close (s);
                continue;
            }
			else if (FD_ISSET (s, &result_set_fds)) {
           
                //  Read command ID.
                unsigned char cmd;
                unsigned char reply;
                ssize_t nbytes = recv (s, &cmd, 1, MSG_WAITALL);
                
                //  Connection closed by peer.
                if (nbytes == 0) {
           
                    //  Unregister all the symbols registered by this connection
                    //  and delete the descriptor from pollfds vector.
                    unregister (s, objects);
                    
                    //  Erase the filedescriptor from fd_list 
	                fd_list.erase (fd_list.begin () + pos);
	                
	                //  Erase the whole list of filedescriptors selectfds
                        //  and add them back without the one erased 
                        //  from fd_list.
	                FD_ZERO (&source_set_fds);
	                for (fd_list_t::size_type i = 0; i < fd_list.size ();
                              i ++)
	                    FD_SET (fd_list[i], &source_set_fds);
                
                    close (s);
                    continue;
                }

                assert (nbytes == 1);

                switch (cmd) {
                case create_id:
                    {
                        
                        //  Parse type ID.
                        unsigned char type_id;
                        nbytes = recv (s, &type_id, 1, MSG_WAITALL);
                        assert (nbytes == 1);

                        //  Parse object name.
                        unsigned char size;
                        nbytes = recv (s, &size, 1, MSG_WAITALL);
                        assert (nbytes == 1);
                        char name [256];
                        nbytes = recv (s, name, size, MSG_WAITALL);
                        assert (nbytes == size);
                        name [size] = 0;

                        //  Parse interface_i.
                        nbytes = recv (s, &size, 1, MSG_WAITALL);
                        assert (nbytes == 1);
                        char interface_i [256];
                        nbytes = recv (s, interface_i, size, MSG_WAITALL);
                        assert (nbytes == size);
                        interface_i [size] = 0;

                        //  Insert object to the repository.
                        object_info_t info = {interface_i, s};
                        if (!objects [type_id].insert (
                              objects_t::value_type (name, info)).second) {

                            //  Send an error if the exchange already exists.
                            reply = fail_id;
                            nbytes = send (s, &reply, 1, 0);
                            assert (nbytes == 1);
#ifdef ZMQ_TRACE
                            printf ("Error when creating object: object %d:%s"
                                " already exists.\n", (int) type_id, name);
#endif
                            break;
                        }

                        //  Send reply command.
                        reply = create_ok_id;
                        nbytes = send (s, &reply, 1, 0);
                        assert (nbytes == 1);
#ifdef ZMQ_TRACE
                        printf ("Object %d:%s created (%s).\n", type_id, name,
                            interface_i);
#endif
                        break;
                    }
                case get_id:
                    {
                        //  Parse type ID.
                        unsigned char type_id;
                        nbytes = recv (s, &type_id, 1, MSG_WAITALL);
                        assert (nbytes == 1);

                        //  Parse object name.
                        unsigned char size;
                        nbytes = recv (s, &size, 1, MSG_WAITALL);
                        assert (nbytes == 1);
                        char name [256];
                        nbytes = recv (s, name, size, MSG_WAITALL);
                        assert (nbytes == size);
                        name [size] = 0;

                        //  Find the exchange in the repository.
                        objects_t::iterator it =
                            objects [type_id].find (name);
                        if (it == objects [type_id].end ()) {

                             //  Send the error.
                             reply = fail_id;
                             nbytes = send (s, &reply, 1, 0);
                             assert (nbytes == 1);
#ifdef ZMQ_TRACE
                             printf ("Error when looking for an object: "
                                 "object %d:%s does not exist.\n",
                                 (int) type_id, name);
#endif
                             break;
                        }

                        //  Send reply command.
                        reply = get_ok_id;
                        nbytes = send (s, &reply, 1, 0);
                        assert (nbytes == 1);

                        //  Send the interface_i.
                        size = it->second.interface_i.size ();
                        nbytes = send (s, &size, 1, 0);
                        assert (nbytes == 1);
                        nbytes = send (
                            s, it->second.interface_i.c_str (), size, 0);
                        assert (nbytes == size);

#ifdef ZMQ_TRACE
                        printf ("Object %d:%s retrieved (%s).\n", (int) type_id,
                            name, it->second.interface_i.c_str ());
#endif
                        break;
                    }
                default:
                    assert (false);
                }
            }

        }

        //  Accept incoming connection.
        if (FD_ISSET (fd_list [0], &result_set_fds)) {
            int s = listening_socket->accept ();
            errno_assert (s != -1);
            
            FD_SET (s, &source_set_fds);
            fd_list.push_back (s);
            if (maxfdp1 <= s)
                maxfdp1 = s + 1;
        }

    }

    return 0;
}

#endif
