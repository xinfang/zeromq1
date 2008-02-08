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

#ifndef __ZMQ_YPOLLSET_HPP_INCLUDED__
#define __ZMQ_YPOLLSET_HPP_INCLUDED__

#if HAVE_CONFIG_H
    #include "config.h"
#endif

#include <assert.h>

#include "i_signaler.hpp"
#include "atomic_uint32.hpp"
#include "ysemaphore.hpp"
#include "stdint.hpp"

namespace zmq
{

    //  ypollset_t class allows for rapid polling for up to 31 different signals
    //  each produced by a different thread.
    class ypollset_t : public i_signaler
    {
        enum {wait_signal = 31};

    public:

        //  Create the pollset
        inline ypollset_t ()
        {
        }

        //  Send a signal to the pollset
        void signal (int signal_);

        //  Wait for signal. Returns a set of signals in form of a bitmap.
        //  Signal with index 0 corresponds to value 1, index 1 to value 2,
        //  index 2 to value 4 etc.
        inline uint32_t poll ()
        {
            uint32_t result = bits.izte (1 << wait_signal, 0);
            if (!result) {
                sem.wait ();
                result = bits.xchg (0);
            }
            return result;      
        }

        //  Same as poll, however, if there is no signal available,
        //  function returns zero immediately instead of waiting for a signal.
        inline uint32_t check ()
        {
            return bits.xchg (0);
        }

    protected:
        atomic_uint32 bits;
        ysemaphore_t sem;
    };

}

#endif
