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

#include "export.hpp"
#include "i_poller.hpp"
#include "i_pollable.hpp"
#include "dispatcher.hpp"
#include "ysocketpair.hpp"
#include "thread.hpp"

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
        bool retired;
    };

    //  The wait() method uses this structure to pass an event to its caller.
    struct event_t {
        int fd;
        enum events name;
        event_source_t *ev_source;
    };

    typedef std::vector <event_t> event_list_t;

    template <class T> class poller_t : public i_poller
    {
    public:

        ZMQ_EXPORT static i_thread *create (dispatcher_t *dispatcher_);

        ZMQ_EXPORT ~poller_t ();

        //  i_poller implementation.
        int get_thread_id ();
        void send_command (i_thread *destination_, const command_t &command_);
        handle_t add_fd (int fd_, i_pollable *engine_);
        void rm_fd (handle_t handle_);
        void set_pollin (handle_t handle_);
        void reset_pollin (handle_t handle_);
        void set_pollout (handle_t handle_);
        void reset_pollout (handle_t handle_);

    private:

        poller_t (dispatcher_t *dispatcher_);

        //  Main worker thread routine.
        static void worker_routine (void *arg_);

        //  Main routine (non-static) - called from worker_routine.
        void loop ();

        void free_retired_event_sources ();

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
        thread_t *worker;

        //  We perform I/O multiplexing using event monitor.
        T event_monitor;

        //  We cannot free event source in rm_fd() method immediately. Rather
        //  we mark it as retired and put it onto retired list. The memory
        //  associated with the event source is released only after all
        //  events returned from wait() method have been processed. This is
        //  necessary to prevent use-after-free error.
        std::vector <event_source_t*> retired_list;
    };

}

template <class T>
zmq::i_thread *zmq::poller_t <T>::create (dispatcher_t *dispatcher_)
{
    return new poller_t <T> (dispatcher_);
}

template <class T>
zmq::poller_t <T>::poller_t (dispatcher_t *dispatcher_) :
    dispatcher (dispatcher_)
{
    ctl_desc.engine = NULL;
    ctl_desc.cookie = event_monitor.add_fd (signaler.get_fd (), &ctl_desc);
    ctl_desc.retired = false;
    event_monitor.set_pollin (ctl_desc.cookie);

    //  Register the thread with command dispatcher.
    thread_id = dispatcher->allocate_thread_id (&signaler);

    //  Create the worker thread.
    worker = new thread_t (worker_routine, this);
}

template <class T>
zmq::poller_t <T>::~poller_t ()
{
    //  Send a 'stop' event to the worker thread.
    //  TODO: Analyse whether using the 'to-self' command pipe here
    //        is appropriate.
    command_t cmd;
    cmd.init_stop ();
    dispatcher->write (thread_id, thread_id, cmd);

    //  Wait till worker thread terminates.
    delete worker;

    dispatcher->deallocate_thread_id (thread_id);

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
zmq::handle_t zmq::poller_t <T>::add_fd (int fd_, i_pollable *engine_)
{
    event_source_t *ev_source = (event_source_t*) malloc (sizeof *ev_source);
    assert (ev_source != NULL);
    ev_source->engine = engine_;
    ev_source->cookie = event_monitor.add_fd (fd_, ev_source);
    ev_source->retired = false;
    return ev_source;
}

template <class T>
void zmq::poller_t <T>::rm_fd (handle_t handle_)
{
    event_source_t *ev_source = (event_source_t*) handle_;
    event_monitor.rm_fd (ev_source->cookie);
    ev_source->retired = true;
    retired_list.push_back (ev_source);
}

template <class T>
void zmq::poller_t <T>::set_pollin (handle_t handle_)
{
    event_source_t *ev_source = (event_source_t*) handle_;
    event_monitor.set_pollin (ev_source->cookie);
}

template <class T>
void zmq::poller_t <T>::reset_pollin (handle_t handle_)
{
    event_source_t *ev_source = (event_source_t*) handle_;
    event_monitor.reset_pollin (ev_source->cookie);
}

template <class T>
void zmq::poller_t <T>::set_pollout (handle_t handle_)
{
    event_source_t *ev_source = (event_source_t*) handle_;
    event_monitor.set_pollout (ev_source->cookie);
}

template <class T>
void zmq::poller_t <T>::reset_pollout (handle_t handle_)
{
    event_source_t *ev_source = (event_source_t*) handle_;
    event_monitor.reset_pollout (ev_source->cookie);
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
    event_list_t events;

    while (true) {
        event_monitor.wait (events);

        for (event_list_t::size_type i = 0; i < events.size (); i ++) {
            event_source_t *ev_source = events [i].ev_source;

            if (ev_source->retired)
                continue;
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
                                return;
                    }
                }
            }
            else {
                switch (events [i].name) {
                case ZMQ_EVENT_OUT:
                    ev_source->engine->out_event ();
                    break;
                case ZMQ_EVENT_IN:
                case ZMQ_EVENT_ERR:
                    ev_source->engine->in_event ();
                    break;
                }
            }
        }

        events.clear ();

        if (!retired_list.empty ())
            free_retired_event_sources ();
    }
}

template <class T>
void zmq::poller_t <T>::free_retired_event_sources ()
{
    while (!retired_list.empty ()) {
        event_source_t *ev_source = retired_list.back ();
        retired_list.pop_back ();
        free (ev_source);
    }
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
