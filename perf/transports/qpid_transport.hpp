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

#ifndef __PERF_QPID_TRANSPORT_HPP_INCLUDED__
#define __PERF_QPID_TRANSPORT_HPP_INCLUDED__

#include "i_transport.hpp"

#include <qpid/client/Connection.h>
#include <qpid/client/Session.h>
#include <qpid/client/Message.h>
#include <qpid/client/LocalQueue.h>
#include <qpid/client/SubscriptionManager.h>

namespace perf
{
    using namespace qpid::client;
    using namespace qpid::framing;

    class qpid_t : public i_transport
    {
    public:
        qpid_t (const char *broker_, bool bind_, 
            const char *up_queue_name_, const char *down_queue_name_)
        {
            //  Setup connection parameters.
            ConnectionSettings settings;
            settings.host = broker_;

            //  Diasble Nagle's algorithm.
            settings.tcpNoDelay = true;

            //  Open connection to the broker.
            connection.open (settings);

            session =  connection.newSession();

            if (bind_) {

                //  Bind to already created global queues ('remote').
                
                //  Bind uplik queue.
                session.exchangeBind (arg::exchange = "amq.direct", 
                    arg::queue = up_queue_name_, 
                    arg::bindingKey = up_queue_name_);

                //  Bind downlink queue.
                session.exchangeBind (arg::exchange = "amq.direct", 
                    arg::queue = down_queue_name_, 
                    arg::bindingKey = down_queue_name_);
               
                //  remote peer routes messages donwlink.
                message.getDeliveryProperties(). 
                    setRoutingKey (down_queue_name_);

                //  remote peer recieves messages from uplink queue
                SubscriptionManager subscriptions (session);
                subscriptions.subscribe (local_queue, string (up_queue_name_));

            } else {
                
                //  Create global queues ('local').

                //  Uplink queue.
                session.queueDeclare (arg::queue = up_queue_name_, 
                    arg::durable = false, arg::autoDelete = true);

                //  Downlink queue.
                session.queueDeclare (arg::queue = down_queue_name_,
                    arg::durable = false, arg::autoDelete = true);

                //  local peer routes messages uplink
                message.getDeliveryProperties(). 
                    setRoutingKey (up_queue_name_);
 
                //  local peer receives messages from donlink
                SubscriptionManager subscriptions (session);
                subscriptions.subscribe (local_queue, string (down_queue_name_));
            }
        }

        ~qpid_t ()
        {
            //  Close connection to the broker.
            connection.close();
        }

        inline virtual void send (size_t size_)
        {
            string data (size_, '.');
            message.setData (data);
            session.messageTransfer (arg::content = message, 
                arg::destination = "amq.direct");
        }

        inline virtual size_t receive ()
        {
            Message msg;
            msg = local_queue.get ();
            return msg.getData().size ();
        }

    protected:
        Connection connection;
        Session session;
        Message message;
        LocalQueue local_queue;
    };
}
#endif
