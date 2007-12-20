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

#ifndef __ZMQ_YPOLLSET_INCLUDED__
#define __ZMQ_YPOLLSET_INCLUDED__

#include <assert.h>
#include <stdint.h>

#include "atomic_uint32.hpp"
#include "semaphore.hpp"

namespace zmq
{

    //  ypollset_t class allows for rapid polling for up to 31 different signals
    //  each produced by a different thread.
    class ypollset_t
    {
        enum {wait_index = 31};

    public:
        inline ypollset_t ()
        {
        }

        inline void signal (int index)
        {
            assert (index >= 0 && index < 31);
            bits.bts (index);
            bool wait = bits.btr (wait_index);
            if (wait)
                sync.post (); 
        }

        inline uint32_t check ()
        {
            return bits.xchg (0);
        }

        inline uint32_t poll ()
        {
            uint32_t result = bits.izte (1 << wait_index, 0);
            if (!result) {
                sync.wait ();
                result = bits.xchg (0);
            }
            return result;      
        }

    protected:
        atomic_uint32 bits;
        semaphore_t sync;
    };

}

#endif
