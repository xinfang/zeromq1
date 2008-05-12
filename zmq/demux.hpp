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

#ifndef __ZMQ_DEMUX_HPP_INCLUDED__
#define __ZMQ_DEMUX_HPP_INCLUDED__

#include <assert.h>
#include <vector>
#include <map>
#include <string>

#include "pipe.hpp"

namespace zmq
{

    class demux_t
    {
    private:

        typedef std::vector <pipe_t*> exchange_t;
        typedef std::map <std::string, exchange_t> exchanges_t;

    public:

        demux_t ();
        ~demux_t ();

        void send_to (const char *exchange_, pipe_t *pipe_);

        inline void write (const char *exchange_, void *msg_)
        {
            //  Find the specified exchange
            exchanges_t::iterator eit = exchanges.find (exchange_);
            assert (eit != exchanges.end ());
            exchange_t &exchange = eit->second;

            //  Optimisation for the case where's there only a single pipe
            //  to send the message to - no refcount adjustment (i.e. atomic
            //  operations) needed.
            if (exchange.size () == 1) {
                (*exchange.begin ())->write (msg_);
                return;
            }

            for (exchange_t::iterator it = exchange.begin ();
                  it != exchange.end (); it ++) {
                void *msg = msg_safe_copy (msg_); 
                (*it)->write (msg);
            }
            msg_dealloc (msg_);
        }

        inline void instant_write (const char *exchange_, void *msg_)
        {
            //  Find the specified exchange
            exchanges_t::iterator eit = exchanges.find (exchange_);
            assert (eit != exchanges.end ());
            exchange_t &exchange = eit->second;

            //  Optimisation for the case where's there only a single pipe
            //  to send the message to - no refcount adjustment (i.e. atomic
            //  operations) needed.
            if (exchange.size () == 1) {
                (*exchange.begin ())->instant_write (msg_);
                return;
            }

            for (exchange_t::iterator it = exchange.begin ();
                  it != exchange.end (); it ++) {
                void *msg = msg_safe_copy (msg_); 
                (*it)->instant_write (msg);
            }
            msg_dealloc (msg_);
        }

        inline void flush ()
        {
            //  In the outer loop traverse all the exchanges,
            //  in the inner loop all the pipes belonging to the exchange
            //  are traversed. Flush is called on each of them.
            for (exchanges_t::iterator eit = exchanges.begin ();
                  eit != exchanges.end (); eit ++)
                for (exchange_t::iterator it = eit->second.begin ();
                      it != eit->second.end (); it ++)
                    (*it)->flush ();
        }

    private:

        exchanges_t exchanges;
    };

}

#endif
