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

#ifndef __PERF_YSUITE_HPP_INCLUDED__
#define __PERF_YSUITE_HPP_INCLUDED__

#include <assert.h>
#include <sys/poll.h>

#include "../interfaces/i_transport.hpp"

#include "../../zmq/err.hpp"
#include "../../zmq/ypipe.hpp"
#include "../../zmq/ysemaphore.hpp"
#include "../../zmq/ysocketpair.hpp"
#include "../../zmq/ypollset.hpp"

namespace perf
{

    enum active_sync_type_t
    {
        active_sync_semaphore,
        active_sync_socketpair,
        active_sync_pollset
    };

    class ysuite_t : public i_transport
    {
    public:
        ysuite_t (active_sync_type_t sync_type_) :
            first (NULL),
            last (NULL),
            sync_type (sync_type_),
            ypipe (false)
        {
            pollset.fd = ysocketpair.get_fd ();
            pollset.events = POLLIN;
        }

        inline ~ysuite_t ()
        {
            while (first != last) {
                zmq::ypipe_t <void*>::item_t *o = first;
                first = first->next;
                delete o;
            }
        }

        inline virtual void send (size_t)
        {
            //  In ysuite, pointers are passed instead of actual data as both
            //  sender and receiver reside in the same memory space.
            if (!ypipe.write (NULL)) {
                switch (sync_type)
                {
                case active_sync_semaphore:
                    ysemaphore.signal (0);
                    break;
                case active_sync_socketpair:
                    ysocketpair.signal (0);
                    break;
                case active_sync_pollset:
                    ypollset.signal (0);
                    break;
                default:
                    assert (0);
                }
            }
        }

        inline virtual size_t receive ()
        {
            if (first == last) {
                switch (sync_type)
                {
                case active_sync_semaphore:
                    if (!ypipe.read (&first, &last)) {
                        ysemaphore.wait ();
                        ypipe.read (&first, &last);
                    }
                    break;
                case active_sync_socketpair:
                    {
                        if (!ypipe.read (&first, &last)) {
                            int rc = poll (&pollset, 1, -1);
                            errno_assert (rc > 0);
                            assert (pollset.revents == POLLIN);
                            ysocketpair.get_signal ();
                            ypipe.read (&first, &last);
                        }
                    }
                    break;
                case active_sync_pollset:
                    if (!ypipe.read (&first, &last)) {
                        ypollset.poll ();
                        ypipe.read (&first, &last);
                    }
                    break;
                default:
                    assert (0);
                }
            }

            zmq::ypipe_t <void*>::item_t *o = first;
            first = first->next;
            return 0;
        }

    protected:
        zmq::ypipe_t <void*> ypipe;
        zmq::ypipe_t <void*>::item_t *first;
        zmq::ypipe_t <void*>::item_t *last;
        active_sync_type_t sync_type;
        zmq::ysemaphore_t ysemaphore;
        zmq::ysocketpair_t ysocketpair;
        pollfd pollset;
        zmq::ypollset_t ypollset;
    };

}

#endif
