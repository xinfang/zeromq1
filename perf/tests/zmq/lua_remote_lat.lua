--
--    Copyright (c) 2007-2009 FastMQ Inc.
--
--    This file is part of 0MQ.
--
--    0MQ is free software; you can redistribute it and/or modify it under
--    the terms of the Lesser GNU General Public License as published by
--    the Free Software Foundation; either version 3 of the License, or
--    (at your option) any later version.
--
--    0MQ is distributed in the hope that it will be useful,
--    but WITHOUT ANY WARRANTY; without even the implied warranty of
--    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
--    Lesser GNU General Public License for more details.
--
--    You should have received a copy of the Lesser GNU General Public License
--    along with this program.  If not, see <http://www.gnu.org/licenses/>.
--

require "libluazmq"

obj = zmq.create("127.0.0.1")

eid = zmq.exchange(obj, "EG", zmq.SCOPE_GLOBAL, "127.0.0.1:5762",
    zmq.STYLE_DATA_DISTRIBUTION)
qid = zmq.queue(obj, "QG", zmq.SCOPE_GLOBAL, "127.0.0.1:5763",
    zmq.NO_LIMIT, zmq.NO_LIMIT, zmq.NO_SWAP)

while 1 do
   obuf = zmq.receive(obj, 1)
   zmq.send(obj, eid, obuf, 1)
   zmq.free(obuf)
end
