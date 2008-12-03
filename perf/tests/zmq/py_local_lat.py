#
#    Copyright (c) 2007-2008 FastMQ Inc.
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

def main ():
    if len (sys.argv) < 4:
        print 'usage: local_lat <hostname> <message-size> <roundtrip-count>'
        sys.exit (1)

    try:
        message_size = int (sys.argv [2])
        roundtrip_count = int (sys.argv [3])
    except (ValueError, OverflowError), e:
        print 'message-size and roundtrip-count must be integers'
        sys.exit (1)

    print "message size:", message_size, "[B]"
    print "roundtrip count:", roundtrip_count

    z = libpyzmq.ZMQ (hostname = sys.argv [1])

    eid = z.create_exchange (exchange_name = 'EL', scope = libpyzmq.SCOPE_LOCAL)
    qid = z.create_queue (queue_name = 'QL', scope = libpyzmq.SCOPE_LOCAL)
    z.bind ('EL', 'QG')
    z.bind ('EG', 'QL')

    msg_out = ''.join ([' ' for n in range (0, message_size)])
    start = datetime.now ()
    for i in range (0, roundtrip_count):
        z.send (eid, msg_out)
        msg_in = z.receive ()
        assert len (msg_in) == message_size
    end = datetime.now ()

    delta = end - start
    delta_us = delta.seconds * 1000000 + delta.microseconds
    print 'Your average latency is', delta_us / roundtrip_count, ' [us]'

if __name__ == "__main__":
    main ()
