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

#include <algorithm>

#include "select_thread.hpp"
#include "err.hpp"

zmq::i_thread *zmq::select_thread_t::create (dispatcher_t *dispatcher_)
{
    select_thread_t *instance = new select_thread_t (dispatcher_);
    assert (instance);
    return instance;
}

zmq::select_thread_t::select_thread_t (dispatcher_t *dispatcher_) :
    dispatcher (dispatcher_)
  
{
    //  Clear file descriptors sets.
    FD_ZERO (&source_set_in);
    FD_ZERO (&source_set_out);
    FD_ZERO (&result_set_in);
    FD_ZERO (&result_set_out);
    FD_ZERO (&error_set);
    
    //  Initialise the set.
    FD_SET (signaler.get_fd () , &source_set_in);
    FD_SET (signaler.get_fd () , &error_set);
    
    //  Add file descriptor of signler into the set.
    fdset.push_back (signaler.get_fd ());
    maxfdp1 = signaler.get_fd () + 1;
    
    //  Register the thread with command dispatcher.
    thread_id = dispatcher->allocate_thread_id (&signaler);

    //  Create the worker thread.
    worker = new thread_t (worker_routine, this);
}

zmq::select_thread_t::~select_thread_t ()
{
    //  Send a 'stop' event ot the worker thread.
    //  TODO: Analyse whether using the 'to-self' command pipe here
    //        is appropriate.
    command_t cmd;
    cmd.init_stop ();
    dispatcher->write (thread_id, thread_id, cmd);

    //  Wait till worker thread terminates.
    delete worker;
}

int zmq::select_thread_t::get_thread_id ()
{
    return thread_id;
}

void zmq::select_thread_t::send_command (i_thread *destination_,
    const command_t &command_)
{
    if (destination_ == (i_thread*) this)
        process_command (command_);
    else
        dispatcher->write (thread_id, destination_->get_thread_id (), command_);
}

zmq::handle_t zmq::select_thread_t::add_fd (int fd_, i_pollable *engine_)
{
    handle_t handle;

    //  Set maxfdp1 as maximum file descriptor plus 1.
    if(maxfdp1 <= fd_)
        maxfdp1 = fd_ + 1;
        
    fdset.push_back(fd_);
   
    engines.push_back (engine_);
    handle.fd = fd_;
    return handle;
}

void zmq::select_thread_t::rm_fd (handle_t handle_)
{
    assert (false);
}

void zmq::select_thread_t::set_pollin (handle_t handle_)
{
    FD_SET (handle_.fd, &source_set_in);
}

void zmq::select_thread_t::reset_pollin (handle_t handle_)
{
     FD_CLR (handle_.fd, &source_set_in);
}

void zmq::select_thread_t::set_pollout (handle_t handle_)
{
    FD_SET (handle_.fd, &source_set_out);
}

void zmq::select_thread_t::reset_pollout (handle_t handle_)
{
    FD_CLR (handle_.fd, &source_set_out);
}

void zmq::select_thread_t::worker_routine (void *arg_)
{
    select_thread_t *self = (select_thread_t*) arg_;
    self->loop ();
}

void zmq::select_thread_t::loop ()
{
    while (true)
    {  
        //  Wait for events.
        memcpy (&result_set_in, &source_set_in, sizeof (source_set_in));
        memcpy (&result_set_out, &source_set_out, sizeof (source_set_out));
        for (fd_set_t::size_type index = 0; index != fdset.size (); 
            index ++) {
            FD_SET (fdset [index], &error_set);        
        }
        
        int rc = 0;
       
        while (rc == 0) { 
            rc = select(maxfdp1, &result_set_in, &result_set_out,
                &error_set, NULL);
#ifdef ZMQ_HAVE_WINDOWS
            wsa_assert (rc != SOCKET_ERROR);
#else
            errno_assert (rc != -1);
#endif
        }

        //  First of all, process commands from other threads.
        if (FD_ISSET(fdset[0], &result_set_in))   {
            uint32_t signals = signaler.check ();
            assert (signals);
            if (!process_commands (signals))
                return;
        }

        //  Process socket errors.
        for (fd_set_t::size_type index = 1; index != fdset.size (); 
            index ++) 
            if (FD_ISSET (fdset [index], &error_set)) 
                engines [index - 1]->error_event ();
           
        //  Process out events from the engines.
        for (fd_set_t::size_type index = 1; index !=fdset.size ();
            index ++) 
            if (FD_ISSET (fdset [index], &result_set_out)) 
               engines [index - 1]->out_event ();
       
        //  Process in events from the engines.
        //  TODO: investigate the POLLHUP issue on OS X
        for (fd_set_t::size_type index = 1; index !=fdset.size();
            index ++) 
            if (FD_ISSET (fdset [index], &result_set_in)) 
                engines [index - 1]->in_event ();
           
    }
}

bool zmq::select_thread_t::process_command (const command_t &command_)
{
    switch (command_.type) {

    //  Exit the working thread.
    case command_t::stop:
        return false;

    //  Register the engine supplied with the poll thread.
    case command_t::register_engine:
        {
            //  Ask engine to register itself.
            i_engine *engine = command_.args.register_engine.engine;
            assert (engine->type () == engine_type_fd);
            ((i_pollable*) engine)->register_event (this);
        }
        return true;

    //  Unregister the engine.
    case command_t::unregister_engine:
        {
            //  Assert that engine still exists.
            //  TODO: We should somehow make sure this won't happen.
            std::vector <i_pollable*>::iterator it = std::find (
                engines.begin (), engines.end (),
                command_.args.unregister_engine.engine);
            assert (it != engines.end ());

            //  Ask engine to unregister itself.
            i_engine *engine = command_.args.unregister_engine.engine;
            assert (engine->type () == engine_type_fd);
            ((i_pollable*) engine)->unregister_event ();
        }
        return true;

    //  Forward the command to the specified engine.
    case command_t::engine_command:
        {
            //  Check whether engine still exists.
            //  TODO: We should somehow make sure this won't happen.
            std::vector <i_pollable*>::iterator it = std::find (
                engines.begin (), engines.end (),
                command_.args.engine_command.engine);

            //  Forward the command to the engine.
            //  TODO: If the engine doesn't exist drop the command.
            //        However, imagine there's another engine
            //        incidentally allocated on the same address.
            if (it != engines.end ())
                command_.args.engine_command.engine->process_command (
                    command_.args.engine_command.command);
        }
        return true;

    //  Unknown command.
    default:
        assert (false);
    }
}

bool zmq::select_thread_t::process_commands (uint32_t signals_)
{
    //  Iterate through all the threads in the process and find out which
    //  of them sent us commands.
    for (int source_thread_id = 0;
          source_thread_id != dispatcher->get_thread_count ();
          source_thread_id ++) {
        if (signals_ & (1 << source_thread_id)) {

            //  Read all the commands from particular thread.
            command_t command;
            while (dispatcher->read (source_thread_id, thread_id, &command))
                if (!process_command (command))
                    return false;
        }
    }
    return true;
}
