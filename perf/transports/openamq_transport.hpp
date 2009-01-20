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

#ifndef __PERF_OPENAMQ_HPP_INCLUDED__
#define __PERF_OPENAMQ_HPP_INCLUDED__

#include "base.h"
#include "amq_client_connection.h"
#include "amq_client_session.h"

#include "i_transport.hpp"

namespace perf
{

    class openamq_t : public i_transport
    {
    public:

        inline openamq_t (const char *host_, const char *send_rk_,
            const char *receive_rk_, bool direct)
        {
	    //  Store routing keys.
            strncpy (send_rk, send_rk_, 256);
	    strncpy (receive_rk, receive_rk_, 256);

            //  Initialise iCL.
            icl_system_initialise (0, NULL);

            //  Open a connection.
            icl_longstr_t *auth_data =
                amq_client_connection_auth_plain ("guest", "guest");
            connection = amq_client_connection_new (
                (char*) host_, "/", auth_data, "perf", 0, 30000);
            assert (connection);
            icl_longstr_destroy (&auth_data);

            //  Switch into direct mode if required.
            if (direct)
                connection->direct = TRUE;

            //  Open a channel.
            session = amq_client_session_new (connection);
            assert (session);

           //  Create a private queue.
           amq_client_session_queue_declare (
               session, 0, NULL, FALSE, FALSE, TRUE, TRUE, NULL);

           //  Bind the queue to the exchange.
           amq_client_session_queue_bind (session, 0, NULL, "amq.direct",
               (char*) receive_rk, NULL);

           //  Consume from the queue.
           amq_client_session_basic_consume (
               session, 0, NULL, NULL, TRUE, TRUE, TRUE, NULL);
        }

        inline ~openamq_t ()
        {
            //  Close the channel.
            amq_client_session_destroy (&session);

            //  Close the connection.
            amq_client_connection_destroy (&connection);

            //  Uninitialise system.
            icl_system_terminate ();
        }

        inline virtual void send (size_t size_)
        {
            //  Create the message body.
            void *message_body = malloc (size_);
            assert (message_body);

            //  Create the message itself.
            amq_content_basic_t *content = amq_content_basic_new ();
            amq_content_basic_set_body (content, message_body, size_, free);

            //  Send the message.
            amq_client_session_basic_publish (session, content, 0,
                "amq.direct", (char*) send_rk, FALSE, FALSE);

            //  Release the message.
            amq_content_basic_unlink (&content);
        }

        inline virtual size_t receive ()
        {
            //  Get next message; if none is available wait for it.
            amq_content_basic_t *content =
                amq_client_session_basic_arrived (session);

            if (!content) {

                //  Wait while next message arrives.
                amq_client_session_wait (session, 0);

                //  Exit the loop if Ctrl+C is encountered.
//  FIXME
//                if (!connection->alive)
//                    assert (false);

                //  Get the message.
                content = amq_client_session_basic_arrived (session);
                assert (content);
            }

            //  Retrieve message size.
            size_t size = amq_content_basic_get_body_size (content);

            //  Destroy the message.
            amq_content_basic_unlink (&content);

            return size;
        }

    protected:
        amq_client_connection_t *connection;
        amq_client_session_t *session;
        char send_rk [256];
        char receive_rk [256];
    };

}

#endif
