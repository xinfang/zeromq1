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
#include <algorithm>

#include "pipe.hpp"

namespace zmq
{

    class demux_t
    {
    public:

        demux_t ();
        ~demux_t ();

        void send_to (pipe_t *pipe_);

        inline void write (void *msg_)
        {
            //  Optimisation for the case where's there only a single pipe
            //  to send the message to - no refcount adjustment (i.e. atomic
            //  operations) needed.
            if (pipes.size () == 1) {
                (*pipes.begin ())->write (msg_);
                return;
            }

            for (pipes_t::iterator it = pipes.begin ();
                  it != pipes.end (); it ++) {
                void *msg = msg_safe_copy (msg_); 
                (*it)->write (msg);
            }
            msg_dealloc (msg_);
        }

        inline void flush ()
        {
            for (pipes_t::iterator it = pipes.begin ();
                  it != pipes.end (); it ++)
                (*it)->flush ();
        }

        inline void terminate_pipes () 
        {
            //  Write delimiters to the pipes
            for (pipes_t::iterator it = pipes.begin ();
                  it != pipes.end (); it ++) {
                (*it)->write (NULL);
                (*it)->flush ();
            }

            //  Remove all pointers to pipes
            pipes.clear ();
        }

        inline void destroy_pipe (pipe_t *pipe_)
        {
            //  Find the pipe
            pipes_t::iterator it = 
                std::find (pipes.begin (), pipes.end (), pipe_);

            //  Send delimiter to the pipe and drop the pointer
            if (it != pipes.end ()) {
                pipe_->write (NULL);
                pipe_->flush ();
                pipes.erase (it);
            }
        }

    private:

        typedef std::vector <pipe_t*> pipes_t;
        pipes_t pipes;
    };

}

#endif
