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

#ifndef __PERF_YSUITE_HPP_INCLUDED__
#define __PERF_YSUITE_HPP_INCLUDED__

#include <assert.h>
#include <sys/poll.h>

#include "i_transport.hpp"

#include <zmq/err.hpp>
#include <zmq/ypipe.hpp>
#include <zmq/ysemaphore.hpp>
#include <zmq/ysocketpair.hpp>
#include <zmq/ypollset.hpp>

namespace perf
{
    enum active_sync_type_t
    {
        active_sync_semaphore,
        active_sync_socketpair,
        active_sync_pollset
    };

    class ysuite_ypipe_t : public i_transport
    {
    public:
        ysuite_ypipe_t (active_sync_type_t sync_type_, 
              zmq::ypipe_t<void*, false> *ypipe_1_,
              zmq::ypipe_t<void*, false> *ypipe_2_,
              zmq::ysemaphore_t *ysemaphore_1_,
              zmq::ysemaphore_t *ysemaphore_2_,
              zmq::ysocketpair_t *ysocketpair_1_,
              zmq::ysocketpair_t *ysocketpair_2_,
              pollfd *pollset_1_,
              pollfd *pollset_2_,
              zmq::ypollset_t *ypollset_1_,
              zmq::ypollset_t *ypollset_2_) :
            sync_type (sync_type_),
            first (NULL),
            last (NULL),
            ypipe_1 (ypipe_1_),
            ypipe_2 (ypipe_2_),
            ysemaphore_1 (ysemaphore_1_),
            ysemaphore_2 (ysemaphore_2_),
            ysocketpair_1 (ysocketpair_1_),
            ysocketpair_2 (ysocketpair_2_),
            pollset_1 (pollset_1_),
            pollset_2 (pollset_2_),
            ypollset_1 (ypollset_1_),
            ypollset_2 (ypollset_2_)
        {

        }

        inline ~ysuite_ypipe_t ()
        {
            while (first != last) {
                zmq::ypipe_t <void*, false>::item_t *o = first;
                first = first->next;
                delete o;
            }
        }
 
        inline virtual void send (size_t)
        {
            //  In ysuite, pointers are passed instead of actual data as both
            //  sender and receiver reside in the same memory space.
            if (!ypipe_1->write (NULL)) {
                switch (sync_type)
                {
                case active_sync_semaphore:
                    ysemaphore_1->signal (0);
                    break;
                case active_sync_socketpair:
                    ysocketpair_1->signal (0);
                    break;
                case active_sync_pollset:
                    ypollset_1->signal (0);
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
                    if (!ypipe_2->read (&first, &last)) {
                        ysemaphore_2->wait ();
                        ypipe_2->read (&first, &last);
                    }
                    break;
                case active_sync_socketpair:
                    {
                        if (!ypipe_2->read (&first, &last)) {
                            int rc = poll (pollset_2, 1, -1);
                            errno_assert (rc > 0);
                            assert (pollset_2->revents == POLLIN);
                            ysocketpair_2->poll ();
                            ypipe_2->read (&first, &last);
                        }
                    }
                    break;
                case active_sync_pollset:
                    if (!ypipe_2->read (&first, &last)) {
                        ypollset_2->poll ();
                        ypipe_2->read (&first, &last);
                    }
                    break;
                default:
                    assert (0);
                }

            }

            first = first->next;
            return 0;
        }
    protected:
        active_sync_type_t sync_type;
        zmq::ypipe_t <void*, false>::item_t *first;
        zmq::ypipe_t <void*, false>::item_t *last;

        zmq::ypipe_t <void*, false> *ypipe_1;
        zmq::ypipe_t <void*, false> *ypipe_2;

        zmq::ysemaphore_t *ysemaphore_1;
        zmq::ysemaphore_t *ysemaphore_2;

        zmq::ysocketpair_t *ysocketpair_1;
        zmq::ysocketpair_t *ysocketpair_2;
        pollfd *pollset_1;
        pollfd *pollset_2;

        zmq::ypollset_t *ypollset_1;
        zmq::ypollset_t *ypollset_2;
    };

    class ysuite_t
    {
    public:
        ysuite_t (active_sync_type_t sync_type_) :
            ypipe_1 (false),
            ypipe_2 (false),
            y_ypipe_1 (sync_type_, &ypipe_1, &ypipe_2, &ysemaphore_1, 
                &ysemaphore_2, &ysocketpair_1, &ysocketpair_2, &pollset_1,
                &pollset_2, &ypollset_1, &ypollset_2),
            y_ypipe_2 (sync_type_, &ypipe_2, &ypipe_1, &ysemaphore_2, 
                &ysemaphore_1, &ysocketpair_2, &ysocketpair_1, &pollset_2, 
                &pollset_1, &ypollset_2, &ypollset_1)
        {
            pollset_1.fd = ysocketpair_1.get_fd ();
            pollset_1.events = POLLIN;
 
            pollset_2.fd = ysocketpair_2.get_fd ();
            pollset_2.events = POLLIN;
        }

        i_transport* get_transport_1 ()
        {
            return &y_ypipe_1;
        }

        i_transport* get_transport_2 ()
        {
            return &y_ypipe_2;
        }

    protected:
        zmq::ysemaphore_t ysemaphore_1;
        zmq::ysemaphore_t ysemaphore_2;

        zmq::ysocketpair_t ysocketpair_1;
        zmq::ysocketpair_t ysocketpair_2;
        pollfd pollset_1;
        pollfd pollset_2;

        zmq::ypollset_t ypollset_1;
        zmq::ypollset_t ypollset_2;

        zmq::ypipe_t <void*, false> ypipe_1;
        zmq::ypipe_t <void*, false> ypipe_2;

        ysuite_ypipe_t y_ypipe_1;
        ysuite_ypipe_t y_ypipe_2;
        
    };
}

#endif
