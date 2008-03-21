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

#ifndef __ZMQ_DISPATCHER_PROXY_HPP_INCLUDED__
#define __ZMQ_DISPATCHER_PROXY_HPP_INCLUDED__

#include <stddef.h>
#include <assert.h>

#include "dispatcher.hpp"

namespace zmq
{

    //  disptcher_proxy can be used to shield the engine from peculiarities
    //  of the communication with dispatcher. Specifically, dispatcher_proxy
    //  handles message batching so that messages are read from/written to
    //  dispatcher in batches rather than one by one, thus making it more
    //  efficient. Moreover, dispatcher_proxy optimises sending messages to
    //  yourself. If the messages is sent to yourself (source engine =
    //  destination engine) the messages is not passed to dispatcher at all.
    //  Instead it is handled locally in dispacher_proxy with no
    //  synchronisation involved.

    //  TODO: message bodies stored in the dispatcher_proxy's writebufs and
    //  readbufs are not deallocated when dispatcher_proxy is destroyed
    class dispatcher_proxy_t
    {
    public:

        typedef dispatcher_t::item_t item_t;

        //  Creates a dispatcher_proxy
        dispatcher_proxy_t (dispatcher_t *dispatcher_);

        //  Destroys the dispatcher proxy
        ~dispatcher_proxy_t ();

        //  Registers engine with the dispatcher.
        inline void set_signaler (i_signaler *signaler_)
        {
            dispatcher->set_signaler (engine_id, signaler_);
        }

        //  Returns true if there is at least single engine that can be polled
        //  to (potentially) provide new messages
        inline bool is_pollable ()
        {
            return (bool) pollable_count;
        }

        //  Returns true if there is at least one engine that can be asked to
        //  provide new messages without previous polling
        inline bool has_messages ()
        {
            return pollable_count < engine_count - 1 ||
                writebufs [engine_id].first ||
                readbufs [engine_id].first != readbufs [engine_id].last;
        }

        //  Write a message. The message will be batched within the proxy
        //  and forwarded to the dispatcher only when 'flush' is called
        void write (int destination_engine_id_, const cmsg_t &value_);

        //  Writes message to the dispatcher immediately (no batching, no need
        //  to call 'flush' method)
        inline void instant_write (int destination_engine_id_,
            const cmsg_t &value_)
        {
            if (destination_engine_id_ == engine_id)
                write (destination_engine_id_, value_);
            else
                dispatcher->write (engine_id, destination_engine_id_, value_);
        }

        //  Flushes all outgoing messages to the dispatcher
        void flush ();

        //  Read a message. Message may be retrieved either from proxy's cache
        //  or directly from the dispatcher if the cache is empty.
        //  The result says whether value was fetched (true) or not (false).
        //  It says *nothing* about whether source of the messages went asleep.
        //  For info about awake/sleeping sources use is_pollable method.
        bool read (int source_engine_id_, cmsg_t *value_);

        //  Marks the engine identified by the ID as being alive - call this
        //  method when you've been notified by the engine that there are
        //  messages available for reading
        inline void revive (int source_engine_id_)
        {
            assert (readbufs [source_engine_id_].pollable);
            readbufs [source_engine_id_].pollable = false;
            pollable_count --;
        }

    private:

        struct writebuf_t
        {
            item_t *first;
            item_t *last;
        };

        struct readbuf_t
        {
            item_t *first;
            item_t *last;
            bool pollable;
        };

        int engine_count;
        int engine_id;
        dispatcher_t *dispatcher;
        int pollable_count;

        writebuf_t *writebufs;
        readbuf_t *readbufs;
    };

}

#endif

