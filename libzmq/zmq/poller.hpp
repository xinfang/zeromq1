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

#ifndef __ZMQ_POLLER_HPP_INCLUDED__
#define __ZMQ_POLLER_HPP_INCLUDED__

#include <vector>
#include <cstdlib>

#include <zmq/export.hpp>
#include <zmq/i_poller.hpp>
#include <zmq/i_pollable.hpp>
#include <zmq/dispatcher.hpp>
#include <zmq/ysocketpair.hpp>
#include <zmq/thread.hpp>

namespace zmq
{

    union cookie_t {
        int fd;
        void *ptr;
    };

    enum events {

        //  The file contains some data.
        ZMQ_EVENT_IN,

        //  The file can accept some more data.
        ZMQ_EVENT_OUT,

        //  There was an error on file descriptor.
        ZMQ_EVENT_ERR
    };

    struct event_source_t {
        i_pollable *engine;
        cookie_t cookie;
    };

    //  The wait() method uses this structure to pass an event to its caller.
    struct event_t {
        int fd;
        enum events name;
        event_source_t *ev_source;
    };

    template <class T> class poller_t : public i_poller
    {
    public:

        ZMQ_EXPORT static i_thread *create (dispatcher_t *dispatcher_);
        
        //  i_poller implementation.
        int get_thread_id ();
        void send_command (i_thread *destination_, const command_t &command_);
        void stop ();
        void destroy ();
        handle_t add_fd (int fd_, i_pollable *engine_);
        void rm_fd (handle_t handle_);
        void set_pollin (handle_t handle_);
        void reset_pollin (handle_t handle_);
        void set_pollout (handle_t handle_);
        void reset_pollout (handle_t handle_);

        //  Callback function called by event_monitor in
        //  process_events () function.
        bool process_event (event_source_t *ev_source, enum events ev_name);

    private:

        poller_t (dispatcher_t *dispatcher_);
        ~poller_t ();

        //  Main worker thread routine.
        static void worker_routine (void *arg_);

        //  Main routine (non-static) - called from worker_routine.
        void loop ();

        //  Processes individual command. Returns false if the thread should
        //  terminate.
        bool process_command (const command_t &command_);

        //  Pointer to dispatcher.
        dispatcher_t *dispatcher;

        //  Thread ID allocated for the poll thread by dispatcher.
        int thread_id;

        //  Poll thread gets notifications about incoming commands using
        //  this socketpair.
        ysocketpair_t signaler;

        //  Event source for signaler socket.
        event_source_t ctl_desc;

        //  Handle of the physical thread doing the I/O work.
        thread_t worker;

        //  We perform I/O multiplexing using event monitor.
        T event_monitor;
    };

}

template <class T>
zmq::i_thread *zmq::poller_t <T>::create (dispatcher_t *dispatcher_)
{
    //  Create the object.
    poller_t <T> *poller = new poller_t <T> (dispatcher_);
    assert (poller);

    //  Start the thread.
    poller->worker.start (worker_routine, poller);

    return poller;
}

template <class T>
void zmq::poller_t <T>::destroy ()
{
    //  Stop the thread.
    worker.stop ();

    //  TODO: At this point terminal handshaking should be done.
    //  Afterwards 'delete this' can be executed. 
}

template <class T>
zmq::poller_t <T>::poller_t (dispatcher_t *dispatcher_) :
    dispatcher (dispatcher_)
{
    ctl_desc.engine = NULL;
    ctl_desc.cookie = event_monitor.add_fd (signaler.get_fd (), &ctl_desc);
    event_monitor.set_pollin (ctl_desc.cookie);

    //  Register the thread with command dispatcher.
    thread_id = dispatcher->allocate_thread_id (this, &signaler);
}

template <class T>
zmq::poller_t <T>::~poller_t ()
{
    event_monitor.rm_fd (ctl_desc.cookie);
}

template <class T>
int zmq::poller_t <T>::get_thread_id ()
{
    return thread_id;
}

template <class T>
void zmq::poller_t <T>::send_command (i_thread *destination_,
    const command_t &command_)
{
    if (destination_ == (i_thread*) this)
        process_command (command_);
    else
        dispatcher->write (thread_id,
            destination_->get_thread_id (), command_);
}

template <class T>
void zmq::poller_t <T>::stop ()
{
    //  'to-self' command pipe is used solely for the 'stop' command.
    //  This way there's no danger of 2 threads accessing the pipe
    //  at the same time.
    command_t cmd;
    cmd.init_stop ();
    dispatcher->write (thread_id, thread_id, cmd);
}

template <class T>
zmq::handle_t zmq::poller_t <T>::add_fd (int fd_, i_pollable *engine_)
{
    event_source_t *ev_source = (event_source_t*) malloc (sizeof *ev_source);
    assert (ev_source != NULL);
    ev_source->engine = engine_;
    ev_source->cookie = event_monitor.add_fd (fd_, ev_source);
    return ev_source;
}

template <class T>
void zmq::poller_t <T>::rm_fd (handle_t handle_)
{
    event_monitor.rm_fd (handle_->cookie);
}

template <class T>
void zmq::poller_t <T>::set_pollin (handle_t handle_)
{
    event_monitor.set_pollin (handle_->cookie);
}

template <class T>
void zmq::poller_t <T>::reset_pollin (handle_t handle_)
{
    event_monitor.reset_pollin (handle_->cookie);
}

template <class T>
void zmq::poller_t <T>::set_pollout (handle_t handle_)
{
    event_monitor.set_pollout (handle_->cookie);
}

template <class T>
void zmq::poller_t <T>::reset_pollout (handle_t handle_)
{
    event_monitor.reset_pollout (handle_->cookie);
}

template <class T>
void zmq::poller_t <T>::worker_routine (void *arg_)
{
    poller_t <T> *self = (poller_t <T>*) arg_;
    self->loop ();
}

template <class T>
void zmq::poller_t <T>::loop ()
{
    while (!event_monitor.process_events (this));
}

template <class T>
bool zmq::poller_t <T>::process_event (event_source_t *ev_source,
    enum events ev_name)
{
    if (ev_source == &ctl_desc) {
        uint32_t signals = signaler.check ();
        assert (signals);

        //  Iterate through all the threads in the process and find out
        //  which of them sent us commands.
        for (int source_thread_id = 0;
              source_thread_id != dispatcher->get_thread_count ();
              source_thread_id ++) {
            if (signals & (1 << source_thread_id)) {

                //  Read all the commands from particular thread.
                command_t command;
                while (dispatcher->read (source_thread_id, thread_id,
                      &command))
                    if (!process_command (command))
                        return true;
            }
        }
    }
    else {
        switch (ev_name) {
        case ZMQ_EVENT_OUT:
            ev_source->engine->out_event ();
            break;
        case ZMQ_EVENT_IN:
        case ZMQ_EVENT_ERR:
            ev_source->engine->in_event ();
            break;
        }
    }
    return false;
}

template <class T>
bool zmq::poller_t <T>::process_command (const command_t &command_)
{
    i_engine *engine;

    switch (command_.type) {

    //  Exit the working thread.
    case command_t::stop:
        break;

    //  Register the engine supplied with the poll thread.
    case command_t::register_engine:

        //  Ask engine to register itself.
        engine = command_.args.register_engine.engine;
        assert (engine->type () == engine_type_fd);
        ((i_pollable*) engine)->register_event (this);
        break;

    //  Unregister the engine.
    case command_t::unregister_engine:

        engine = command_.args.unregister_engine.engine;
        assert (engine->type () == engine_type_fd);
        ((i_pollable*) engine)->unregister_event ();
        break;

    //  Forward the command to the specified engine.
    case command_t::engine_command:

        //  Forward the command to the engine.
        engine = command_.args.engine_command.engine;
        engine->process_command (
            command_.args.engine_command.command);
        break;

    //  Unknown command.
    default:
        assert (false);
    }

    if (command_.type == command_t::stop)
        return false;
    return true;
}

#endif
