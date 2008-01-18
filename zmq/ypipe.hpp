/*
    Copyright (c) 2007 FastMQ Inc.

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

#ifndef __ZMQ_YPIPE_HPP_INCLUDED__
#define __ZMQ_YPIPE_HPP_INCLUDED__

#include "atomic_ptr.hpp"

namespace zmq
{

    //  Lock-free and wait free queue implementation.
    //  Additionally, ypipe allows to read items in batches to avoid
    //  redundant atomic operations. It also signals to both sender
    //  and receiver if read past the end of the queue was attempted.
    //  Only a single thread can read from the pipe at any specific moment.
    //  Only a single thread can write to the pipe at any specific moment.
    template <typename T> class ypipe_t
    {
    public:

        //  Structure to hold one item in the linked list of T's
        struct item_t
        {
            T value;
            item_t *next;
        };

        //  Initialises the pipe. If 'dead' is set to true, the pipe is
        //  created in 'dead' state.
        ypipe_t (bool dead_ = false)
        {
            r = w = new item_t;
            c.set (dead_ ? NULL : w);
        }

        //  Destroys the pipe
        ~ypipe_t ()
        {
            while (r != w) {
                 item_t *o = r;
                 r = r->next;
                 delete o;
            }
            delete r;
        }

        //  Write an item to the pipe. If pipe is in 'dead' state, function
        //  returns false. Otherwise it returns true. Writing an item to the
        //  pipe revives it in case it is dead. Writing an item to a dead pipe
        //  enqueues the item if 'write_always' is true. Otherwise it
        //  leaves the pipe as is.
        bool write (const T &value_, bool write_always_ = true)
        {
            item_t *n = new item_t;
            w->value = value_;
            w->next = n;
            if (c.cas (w, n) == w) {
                w = n;
                return true;
            }
            else {
                if (write_always_) {
                    w = n;
                    c.set (n);
                }
                else
                    delete n;
                return false;
            }
        }

        //  Write multiple items to the pipe. If pipe is in dead state, function
        //  returns false. Otherwise it returns true. Writing an item to the
        //  pipe revives it in case it is dead. Writing an item to a dead pipe
        //  enqueues the item nontheless.
        bool write (item_t *first_, item_t *last_)
        {
            item_t *n = new item_t;
            last_->next = n;
            
            *w = *first_;
            delete first_;

            if (c.cas (w, n) == w) {
                w = n;
                return true;
            }
            else {
                w = n;
                c.set (n);
                return false;
            }
        }

        //  Reads all the items from the pipe. If there is no item in the pipe,
        //  function returns false and the pipe 'dies'. Otherwise, 'first_'
        //  points to first retrieved item, 'last_' points one past the last
        //  retrieved item. The caller is responsible for deallocation of
        //  the retrieved items afterwards.
        bool read (item_t **first_, item_t **last_)
        {
            *first_ = r;
            r = *last_ = c.cas (r, NULL);
            return *first_ != *last_;
        }

    protected:

        item_t *r;
        item_t *w;
        atomic_ptr <item_t> c;
    };

}

#endif
