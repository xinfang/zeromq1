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
        print 'usage: py_remote_thr <hostname> <message-size> <message-count>'
        sys.exit (1)

    try:
        message_size = int (sys.argv [2])
        message_count = int (sys.argv [3])
    except (ValueError, OverflowError), e:
        print 'message-size and message-count must be integers'
        sys.exit (1)

    z = libpyzmq.ZMQ (host = sys.argv [1])

    eid = z.create_exchange (name = 'EL', scope = libpyzmq.SCOPE_LOCAL, 
			style = libpyzmq.STYLE_LOAD_BALANCING)
    z.bind ('EL', 'QG')

    msg = ''.join ([' ' for n in range (0, message_size)])
    for i in range (0, message_count):
        z.send (eid, msg, True)

    time.sleep (2)

if __name__ == "__main__":
    main ()
