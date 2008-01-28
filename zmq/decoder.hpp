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

#ifndef __ZMQ_DECODER_HPP_INCLUDED__
#define __ZMQ_DECODER_HPP_INCLUDED__

#include <stddef.h>
#include <algorithm>

namespace zmq
{

    template <typename T> class decoder_t
    {
    public:

        inline decoder_t () :
            read_ptr (NULL),
            to_read (0),
            next (NULL)
        {
        }

        inline void write (unsigned char *data_, size_t size_)
        {
            size_t pos = 0;
            while (true) {
                size_t to_copy = std::min (to_read, size_ - pos);
                if (read_ptr) {
                    memcpy (read_ptr, data_ + pos, to_copy);
                    read_ptr += to_copy;
                }
                pos += to_copy;
                to_read -= to_copy;
                if (!to_read)
                    (static_cast <T*> (this)->*next) ();
                if (pos == size_)
                    break;
            }
        }

    protected:

        typedef void (T::*step_t) ();

        inline void next_step (void *read_ptr_, size_t to_read_,
            step_t next_)
        {
            read_ptr = (unsigned char*) read_ptr_;
            to_read = to_read_;
            next = next_;
        }

    private:

        size_t to_read;
        unsigned char *read_ptr;
        step_t next;
    };

}

#endif
