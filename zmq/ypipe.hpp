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

#ifndef __ZMQ_YPIPE_HPP_INCLUDED__
#define __ZMQ_YPIPE_HPP_INCLUDED__

#include "atomic_ptr.hpp"

namespace zmq
{

    //  Lock-free and wait free queue implementation.
    //  Additionally, ypipe allows to read items in batches to avoid
    //  redundant atomic operations.
    //  Only a single thread can read from the pipe at any specific moment.
    //  Only a single thread can write to the pipe at any specific moment.
    //  If the templete parameter D (die-fast) is set to true, the pipe
    //  'dies' immediately after each read. If it is set to false, it dies
    //  only if read method returns no data.

    template <typename T, bool D> class ypipe_t
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
        ypipe_t (bool dead_ = true)
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
        //  pipe revives it in case it is dead.
        //  If 'second' and 'last' arguments are used, you can add an additional
        //  list of linked items into the ypipe. Individual items in the list
        //  must be allocated using new operator.
        bool write (const T &value_, item_t *second_ = NULL,
            item_t *last_ = NULL)
        {
            w->value = value_;
            item_t *n = new item_t;
            
            if (second_) {
                w->next = second_;
                last_->next = n;
            }
            else
                w->next = n;

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

        //  Reads all the items from the pipe.
        //  After the call, 'first_' points to first retrieved item,
        //  'last_' points one past the last retrieved item
        //  (i.e. if first_==last_, no items were retrieved). The caller
        //  is responsible for deallocation of the retrieved items afterwards.
        //  The pipe 'dies' and function returns false:
        //  1. if D==true
        //  2. if D==false and there are no items in the pipe
        bool read (item_t **first_, item_t **last_)
        {
            //  Note that as D is a template parameter, one of the branches
            //  of the following conditional statement will be optimised out.
            if (D) {
                *first_ = r;
                r = *last_ = c.xchg (NULL);
                return false;
            }
            else {
                *first_ = r;
                r = *last_ = c.cas (r, NULL);
                return *first_ != *last_;
            }
        }

    protected:

        item_t *r;
        item_t *w;
        atomic_ptr_t <item_t> c;
    };

}

#endif
