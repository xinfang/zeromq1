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

#include "poll_thread.hpp"
#include "err.hpp"

zmq::poll_thread_t::poll_thread_t (dispatcher_t *dispatcher_,
      i_pollable *engine_) :
    dispatcher (dispatcher_),
    engine (engine_)
{
    //  Register the thread with command dispatcher
    thread_id = dispatcher->allocate_thread_id (&signaler);

    //  Create the worker thread
    int rc = pthread_create (&worker, NULL, worker_routine, this);
    errno_assert (rc == 0);
}

zmq::poll_thread_t::~poll_thread_t ()
{
    //  Send a 'stop' event ot the worker thread
    //  TODO: Analyse whether using the to-self command pipe here is appropriate
    command_t cmd;
    cmd.init_stop ();
    dispatcher->write (thread_id, thread_id, cmd);

    //  Wait till worker thread terminates
    int rc = pthread_join (worker, NULL);
    errno_assert (rc == 0);
}

int zmq::poll_thread_t::get_thread_id ()
{
    return thread_id;
}

void zmq::poll_thread_t::send_command (int destination_thread_id_,
    const command_t &command_)
{
    assert (false);
}

void *zmq::poll_thread_t::worker_routine (void *arg_)
{
    poll_thread_t *self = (poll_thread_t*) arg_;
    self->loop ();
    return 0;
}

void zmq::poll_thread_t::loop ()
{
    bool stop = false;

    pollfd pfd [2];

    //  Poll for administrative commands (revive & stop commands)
    pfd [0].fd = signaler.get_fd ();
    pfd [0].events = POLLIN;

    //  Poll for events from the engine
    pfd [1].fd = engine->get_fd ();

    while (true)
    {
        //  Adjust the events to wait - the engine chooses the events
        pfd [1].events = engine->get_events ();

        //  Wait for events
        int rc = poll (pfd, 2, -1);
        errno_assert (rc != -1);
        assert (!(pfd [0].revents & (POLLERR | POLLHUP | POLLNVAL)));
        assert (!(pfd [1].revents & (POLLERR | POLLNVAL)));

        //  Process commands from other threads
        if (pfd [0].revents & POLLIN) {
            uint32_t signals = signaler.check ();
            assert (signals);
            if (!process_commands (signals))
                return;
        }

        if (pfd [1].revents & POLLOUT) {

            //  Process out event from the engine
            engine->out_event ();
            if (stop && !(engine->get_events () & POLLOUT))
                return;
        }

        if (pfd [1].revents & (POLLIN | POLLHUP)) {

            //  Process in event from the engine
            engine->in_event ();
        }
    }
}

bool zmq::poll_thread_t::process_commands (uint32_t signals_)
{
    for (int source_thread_id = 0;
          source_thread_id != dispatcher->get_thread_count ();
          source_thread_id ++) {
        if (signals_ & (1 << source_thread_id)) {
            dispatcher_t::item_t *first;
            dispatcher_t::item_t *last;
            dispatcher->read (source_thread_id, thread_id, &first, &last);
            while (first != last) {
                switch (first->value.type) {

                //  Exit the working thread
                case command_t::stop:
                    return false;

                //  Forward the command to the specified engine
                case command_t::engine_command:
                    first->value.args.engine_command.engine->process_command (
                        first->value.args.engine_command.command);
                    break;

                //  Unknown command
                default:
                    assert (false);
                }
                dispatcher_t::item_t *o = first;
                first = first->next;
                delete o;
            }
        }
    }
    return true;
}
