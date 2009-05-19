#
#    Copyright (c) 2007-2009 FastMQ Inc.
#
#    This file is part of 0MQ.
#
#    0MQ is free software; you can redistribute it and/or modify it under
#    the terms of the Lesser GNU General Public License as published by
#    the Free Software Foundation; either version 3 of the License, or
#    (at your option) any later version.
#
#    0MQ is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    Lesser GNU General Public License for more details.
#
#    You should have received a copy of the Lesser GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

import sys
from datetime import datetime
import libpyzmq
import time


def main ():
    if len (sys.argv) < 4:
        print 'usage: py_local_lat <hostname> <message-size> <roundtrip-count>'
        sys.exit (1)

    try:
        message_size = int (sys.argv [2])
        roundtrip_count = int (sys.argv [3])
    except (ValueError, OverflowError), e:
        print 'message-size and roundtrip-count must be integers'
        sys.exit (1)

    print "message size:", message_size, "[B]"
    print "roundtrip count:", roundtrip_count

    z = libpyzmq.Zmq (host = sys.argv [1])

    exchange = z.create_exchange (name = 'EL', scope = libpyzmq.SCOPE_LOCAL, 
		style = libpyzmq.STYLE_LOAD_BALANCING)
    queue = z.create_queue (name = 'QL', scope = libpyzmq.SCOPE_LOCAL, location = "",
		hwm = libpyzmq.NO_LIMIT, lwm = libpyzmq.NO_LIMIT, swap = libpyzmq.NO_SWAP)
    z.bind ('EL', 'QG')
    z.bind ('EG', 'QL')

    msg_out = ''.join ([' ' for n in range (0, message_size)])
    start = datetime.now ()
    for i in range (0, roundtrip_count):
        z.send (exchange , msg_out, True)
        list = z.receive (True)
        msg_in = list [1]
        msg_size = list [2]
        assert msg_size == message_size
    end = datetime.now ()

    delta = end - start
    delta_us = delta.seconds * 1000000 + delta.microseconds
    print 'Your average latency is', delta_us / roundtrip_count, ' [us]'

if __name__ == "__main__":
    main ()
