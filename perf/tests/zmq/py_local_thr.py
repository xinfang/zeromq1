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

def main ():
    if len (sys.argv) < 5:
        print ('usage: py_local_thr <hostname> <interface> <message-size> ' +
            '<message-count>')
        sys.exit (1)

    try:
        message_size = int (sys.argv [3])
        message_count = int (sys.argv [4])
    except (ValueError, OverflowError), e:
        print 'message-size and message-count must be integers'
        sys.exit (1)

    print "message size:", message_size, "[B]"
    print "message count:", message_count

    z = libpyzmq.ZMQ (hostname = sys.argv [1])

    z.create_queue (queue_name = 'QG', scope = libpyzmq.SCOPE_GLOBAL,
        interface = sys.argv [2])

    msg = z.receive ()
    assert len (msg) == message_size
    start = datetime.now ()
    for i in range (1, message_count):
        msg = z.receive ()
        assert len (msg) == message_size
    end = datetime.now()

    delta = end - start
    delta_us = delta.seconds * 1000000 + delta.microseconds
    message_thr = (1000000.0 * float (message_count)) / float (delta_us)
    megabit_thr = (message_thr * float (message_size) * 8.0) / 1000000.0;

    print "Your average throughput is %.0f [msg/s]" % (message_thr, )
    print "Your average throughput is %.2f [Mb/s]" % (megabit_thr, )

if __name__ == "__main__":
    main ()
