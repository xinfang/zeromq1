/*
    Copyright (c) 2007-2009 FastMQ Inc.

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

#include <zmq/api_thread.hpp>
#include <zmq/config.hpp>
#include <zmq/err.hpp>

zmq::api_thread_t *zmq::api_thread_t::create (dispatcher_t *dispatcher_,
    i_locator *locator_)
{
    return new api_thread_t (dispatcher_, locator_);
}

zmq::api_thread_t::api_thread_t (dispatcher_t *dispatcher_,
      i_locator *locator_) :
    ticks (0),
    dispatcher (dispatcher_),
    locator (locator_),
    current_queue (0),
    message_mask (message_data)
{
#if defined ZMQ_HAVE_RDTSC_IN_API_THREAD
    last_command_time = 0;
#endif

    //  Register the thread with the command dispatcher.
    thread_id = dispatcher->allocate_thread_id (this, &pollset);
}

zmq::api_thread_t::~api_thread_t ()
{
}

void zmq::api_thread_t::mask (uint32_t notifications_)
{
    message_mask = notifications_ | message_data;
}

int zmq::api_thread_t::create_exchange (const char *name_,
    scope_t scope_, const char *location_,
    i_thread *listener_thread_, int handler_thread_count_,
    i_thread **handler_threads_, style_t style_)
{
    zmq_assert (scope_ == scope_local || scope_ == scope_process ||
        scope_ == scope_global);

    //  Insert the exchange to the local list of exchanges.
    //  Make sure that the exchange doesn't already exist.
    for (exchanges_t::iterator it = exchanges.begin ();
          it != exchanges.end (); it ++)
        zmq_assert (it->first != name_);

    //  Create demux for out_engine.
    i_demux *demux;

    if (style_ == style_load_balancing)
        demux = new load_balancer_t ();
    else
        demux = new data_distributor_t ();

    out_engine_t *engine = out_engine_t::create (demux);
    engine->start (this, this);
    exchanges.push_back (exchanges_t::value_type (name_, engine));

    //  If the scope of the exchange is local, we won't register it
    //  with the locator.
    if (scope_ == scope_local)
        return exchanges.size () - 1;

    //  Register the exchange with the locator.
    dispatcher->create (locator, this, true, name_, this, engine,
        scope_, location_, listener_thread_,
        handler_thread_count_, handler_threads_);

    return exchanges.size () - 1;
}

int zmq::api_thread_t::create_queue (const char *name_, scope_t scope_,
    const char *location_, i_thread *listener_thread_,
    int handler_thread_count_, i_thread **handler_threads_,
    int64_t hwm_, int64_t lwm_, uint64_t swap_)
{
    zmq_assert (scope_ == scope_local || scope_ == scope_process ||
        scope_ == scope_global);

    //  Insert the queue to the local list of queues.
    //  Make sure that the queue doesn't already exist.
    for (queues_t::iterator it = queues.begin ();
          it != queues.end (); it ++)
        zmq_assert (it->first != name_);

    //  Create mux object for in_engine.
    mux_t *mux = new mux_t (hwm_, lwm_, swap_);

    in_engine_t *engine = in_engine_t::create (mux);
    engine->start (this, this);
    queues.push_back (queues_t::value_type (name_, engine));

    //  If the scope of the queue is local, we won't register it
    //  with the locator.
    if (scope_ == scope_local)
        return queues.size ();

    //  Register the queue with the locator.
    dispatcher->create (locator, this, false, name_, this, engine,
        scope_, location_, listener_thread_, handler_thread_count_,
        handler_threads_);

    return queues.size ();
}

void zmq::api_thread_t::bind (const char *exchange_name_,
    const char *queue_name_, i_thread *exchange_thread_,
    i_thread *queue_thread_, const char *exchange_options_,
    const char *queue_options_)
{
    //  Find the exchange.
    i_thread *exchange_thread;
    i_engine *exchange_engine;
    exchanges_t::iterator eit;
    for (eit = exchanges.begin (); eit != exchanges.end (); eit ++)
        if (eit->first == exchange_name_)
            break;
    if (eit != exchanges.end ()) {
        exchange_thread = this;
        exchange_engine = eit->second;
    }
    else {
        //  Proxy engine for global exchange is receiver /sender_ = false/.
        dispatcher->get (locator, this, false, exchange_name_, &exchange_thread,
            &exchange_engine, exchange_thread_, queue_name_, exchange_options_);
    }

    //  Find the queue.
    i_thread *queue_thread;
    i_engine *queue_engine;
    queues_t::iterator qit;
    for (qit = queues.begin (); qit != queues.end (); qit ++)
        if (qit->first == queue_name_)
            break;
    if (qit != queues.end ()) {
        queue_thread = this;
        queue_engine = qit->second;
    }
    else {
        //  Proxy engine for global queue is sender /sender_ = true/.
        dispatcher->get (locator, this, true, queue_name_, &queue_thread,
            &queue_engine, queue_thread_, exchange_name_, queue_options_);
    }

    //  Create the pipe.
    i_demux *demux = exchange_engine->get_demux ();
    i_mux *mux = queue_engine->get_mux ();
    pipe_t *pipe = new pipe_t (exchange_thread, demux,
        queue_thread, mux, mux->get_swap_size ());
    zmq_assert (pipe);

    //  Bind the source end of the pipe.
    command_t demux_cmd;
    demux_cmd.init_attach_pipe_to_demux (demux, pipe);
    send_command (exchange_thread, demux_cmd);

    //  Bind the destination end of the pipe.
    command_t mux_cmd;
    mux_cmd.init_attach_pipe_to_mux (mux, pipe);
    send_command (queue_thread, mux_cmd);
}

bool zmq::api_thread_t::send (int exchange_, message_t &message_, bool block_)
{
    //  Only data messages can be sent. Notifications are intended for notifying
    //  the client about different events rather than for passing them around.
    zmq_assert (message_.type () == message_data);

    //  Process pending commands, if any.
    process_commands ();

    //  Try to send the message.
    bool sent = exchanges [exchange_].second->write (message_);

    if (block_) {

        //  Oops, we couldn't send the message. Wait for the next
        //  command, process it and try to send the message again.
        while (!sent) {
            process_commands (pollset.poll ());
            sent = exchanges [exchange_].second->write (message_);
        }
    }

    //  Flush the message to the pipe.
    //  TODO: This is inefficient in the case of load-balancing mode. Message
    //  is written to a single pipe, however, the flush is done on all the
    //  pipes.
    exchanges [exchange_].second->flush ();

    return sent;
}

bool zmq::api_thread_t::presend (int exchange_, message_t &message_,
    bool block_)
{
    //  Only data messages can be sent. Notifications are intended for notifying
    //  the client about different events rather than for passing them around.
    zmq_assert (message_.type () == message_data);

    //  Try to send the message.
    bool sent = exchanges [exchange_].second->write (message_);

    if (block_) {

        //  We couldn't send the message. Process all available commands
        //  and try to send the message again. If there is no command
        //  available, wait for one.
        while (!sent) {
            process_commands (pollset.poll ());
            sent = exchanges [exchange_].second->write (message_);
        }
    }

    return sent;
}

void zmq::api_thread_t::flush ()
{
    //  Process pending commands, if any.
    process_commands ();

    //  Flush all the exchanges.
    for (exchanges_t::iterator it = exchanges.begin ();
          it != exchanges.end (); it ++)
        it->second->flush ();
}

int zmq::api_thread_t::receive (message_t *message_, bool block_)
{
    bool retrieved = false;
    int qid = 0;

    //  Remember the original current queue position. We'll use this value
    //  as a marker for identifying whether we've inspected all the queues.
    queues_t::size_type start = current_queue;

    //  Big loop - iteration happens each time after new commands
    //  are processed (thus new messages may be available).
    while (true) {

        //  Small loop doesn't make sense if there are no queues.
        if (!queues.empty ()) {

            //  Small loop - we are iterating over the array of queues
            //  checking whether any of them has messages available.
            while (true) {

               //  Get a message or notification that user is subscribed for.
               while (true) {
                   retrieved = queues [current_queue].second->read (message_);
                   if (!retrieved || (message_->type () & message_mask))
                       break;
               }

               if (retrieved)
                   qid = current_queue + 1;

               //  Move to the next queue.
               current_queue ++;
               if (current_queue == queues.size ())
                   current_queue = 0;

               //  If we have a message exit the small loop.
               if (retrieved)
                   break;

               //  If we've iterated over all the queues exit the loop.
               if (current_queue == start)
                   break;
            }
        }

        //  If we have a message exit the big loop.
        if (retrieved)
            break;

        //  If we don't have a message and no blocking is required
        //  skip the big loop.
        if (!block_)
            break;

        //  This is a blocking call and we have no messages
        //  We wait for commands, we process them and we continue
        //  with getting the messages.
        ypollset_t::integer_t signals = pollset.poll ();
        zmq_assert (signals);
        process_commands (signals);
        ticks = 0;
    }

    //  Once every api_thread_poll_rate messages check for signals and process
    //  incoming commands. This happens only if we are not polling altogether
    //  because there are messages available all the time. If poll occurs,
    //  ticks is set to zero and thus we avoid this code.
    if (++ ticks == api_thread_poll_rate) {
        ypollset_t::integer_t signals = pollset.check ();
        if (signals)
            process_commands (signals);
        ticks = 0;
    }

    return qid;
}

zmq::dispatcher_t *zmq::api_thread_t::get_dispatcher ()
{
    return dispatcher;
}

int zmq::api_thread_t::get_thread_id ()
{
    return thread_id;
}

void zmq::api_thread_t::send_command (i_thread *destination_,
    const command_t &command_)
{
    if (destination_ == (i_thread*) this)
        process_command (command_);
    else
        dispatcher->write (thread_id, destination_->get_thread_id (), command_);
}

void zmq::api_thread_t::stop ()
{
    //  There's nothing to do to stop the API thread. API (application) threads
    //  should be already stopped by the time dispatcher is being destroyed.
}

void zmq::api_thread_t::destroy ()
{
    //  TODO: Terminal handshaking should be done at this point.
    //  Afterwards 'delete this' can be executed.
}

void zmq::api_thread_t::process_command (const command_t &command_)
{
    pipe_t *pipe;

    switch (command_.type) {

    case command_t::attach_pipe_to_demux:
        pipe = command_.args.attach_pipe_to_demux.pipe;
        command_.args.attach_pipe_to_demux.demux->send_to (pipe);
        break;

    case command_t::attach_pipe_to_mux:
        pipe = command_.args.attach_pipe_to_mux.pipe;
        command_.args.attach_pipe_to_mux.mux->receive_from (pipe);
        break;

    case command_t::revive_reader:
        pipe = command_.args.revive_reader.pipe;
        pipe->revive_reader ();
        break;

    case command_t::notify_writer:
        pipe = command_.args.notify_writer.pipe;
        pipe->notify_writer (command_.args.notify_writer.position);
        break;

    case command_t::terminate_pipe_req:
        pipe = command_.args.terminate_pipe_req.pipe;
        pipe->terminate_pipe_req ();
        break;

    case command_t::terminate_pipe_ack:
        pipe = command_.args.terminate_pipe_ack.pipe;
        pipe->terminate_pipe_ack ();
        break;

    //  Unsupported/unknown command.
    default:
        zmq_assert (false);
    }
}

void zmq::api_thread_t::process_commands (ypollset_t::integer_t signals_)
{
    for (int source_thread_id = 0;
          source_thread_id != dispatcher->get_thread_count ();
          source_thread_id ++) {
        if (signals_ & (1 << source_thread_id)) {
            command_t command;
            while (dispatcher->read (source_thread_id, thread_id, &command))
                process_command (command);
        }
    }
}

void zmq::api_thread_t::process_commands ()
{
#if defined ZMQ_HAVE_RDTSC_IN_API_THREAD

    //  Optimised version of send doesn't have to check for incoming commands
    //  each time send is called. It does so onlt if certain time elapsed since
    //  last command processing. Command delay varies depending on CPU speed:
    //  It's ~1ms on 3GHz CPU, ~2ms on 1.5GHz CPU etc.

    //  Get timestamp counter.
#if defined __GNUC__
    uint32_t low;
    uint32_t high;
    __asm__ volatile ("rdtsc"
        : "=a" (low), "=d" (high));
    uint64_t current_time = (uint64_t) high << 32 | low;
#elif defined _MSC_VER
    uint64_t current_time = __rdtsc ();
#elif
#error
#endif

	//  Check whether certain time have elapsed since last command processing.
    if (current_time - last_command_time <= api_thread_max_command_delay)
        return;
    last_command_time = current_time;
#endif

    ypollset_t::integer_t signals = pollset.check ();
    if (signals)
        process_commands (signals);
}
