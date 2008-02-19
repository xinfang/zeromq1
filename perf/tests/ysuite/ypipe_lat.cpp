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

#include <unistd.h>

#include "../../transports/ysuite.hpp"
#include "../../workers/echo.hpp"
#include "../../workers/ping_pong.hpp"

#include "test.hpp"

void *worker_function (void*);

struct worker_args_t
{
    int id;
    int msg_size;
    int msg_count;
    perf::i_transport *transport;
};

int main (void) {

    perf::ysuite_t transport (perf::active_sync_semaphore);
//    perf::ysuite_t transport (perf::active_sync_socketpair);
//    perf::ysuite_t transport (perf::active_sync_pollset);

    perf::echo_t receiver (TEST_MSG_COUNT_LAT);

    worker_args_t w_args;

    w_args.id = 0;
    w_args.msg_size = 0;
    w_args.msg_count = TEST_MSG_COUNT_LAT;
    w_args.transport = transport.get_transport_1 ();

    pthread_t worker;

    int rc = pthread_create (&worker, NULL, worker_function, 
            (void*)&w_args);
        assert (rc == 0);


    receiver.run (*transport.get_transport_2 (), "1_0_");

    rc = pthread_join (worker, NULL);
    assert (rc == 0);

    return 0;
}


void *worker_function (void *args_)
{
    // args struct
    worker_args_t *w_args = (worker_args_t*)args_;
    
    perf::ping_pong_t sender (w_args->msg_count, w_args->msg_size);

    sender.run (*w_args->transport, "1_0_");

    return NULL;
}

