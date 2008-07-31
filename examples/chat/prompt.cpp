/*
    Copyright (c) 2007-2008 FastMQ Inc.

    This file is part of 0MQ.

    0MQ is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    0MQ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <string.h>
#include <string>
#include <iostream>

using namespace std;

#include "../../zmq/dispatcher.hpp"
#include "../../zmq/locator.hpp"
#include "../../zmq/poll_thread.hpp"
#include "../../zmq/api_thread.hpp"
#include "../../zmq/message.hpp"

using namespace zmq;

int main (int argc, const char *argv [])
{
    //  Check the command line syntax.
    if (argc != 5) {
        cout << "usage: prompt <locator IP> <locator port> <chatroom name> "
            "<user name>\n" << endl;
        return 1;
    }

    //  Retrieve command line arguments
    const char *locator_ip = argv [1];
    uint16_t locator_port = atoi (argv [2]);
    const char *chatroom_name = argv [3];
    const char *user_name = argv [4];

    //  Initialise 0MQ infrastructure.
    dispatcher_t dispatcher (2);
    locator_t locator (locator_ip, locator_port);
    poll_thread_t *pt = poll_thread_t::create (&dispatcher);
    api_thread_t *api = api_thread_t::create (&dispatcher, &locator);

    //  Create local exchange to send messages to.
    int eid = api->create_exchange ("E", scope_local);

    //  Bind the exchange to globally accessible queue exposed by chatroom.
    //  If the queue is not available, assert.
    char tmp [16];
    snprintf (tmp, 16, "Q_%s", chatroom_name);
    bool ok = api->bind ("E", tmp, pt, pt);
    assert (ok);
    
    while (true) {

        //  Allow user to input the message text. Prepend it by user name.
        char textbuf [1024];
        fgets (textbuf, sizeof (textbuf), stdin);
        string text (user_name);
        text = text + ": " + textbuf;

        //  Create the message (terminating zero is part of the message)
        message_t message (text.size () + 1);
        memcpy (message.data (), text.c_str (), text.size () + 1);

        //  Send the message
        api->send (eid, message);
    }
}
