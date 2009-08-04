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

bytes = arg[1]
count = arg[2]

obj = zmq.create("127.0.0.1")

eid = zmq.exchange(obj, "EL", zmq.SCOPE_LOCAL, "",
    zmq.STYLE_DATA_DISTRIBUTION)
qid = zmq.queue(obj, "QL", zmq.SCOPE_LOCAL, "",
    zmq.NO_LIMIT, zmq.NO_LIMIT, zmq.NO_SWAP)

zmq.bind(obj, "EL", "QG", "", "")
zmq.bind(obj, "EG", "QL", "", "")


ibuf = zmq.buffer(bytes)

t0 = zmq.now()

for i = 1, count do
   zmq.send(obj, eid, ibuf, 1)
   obuf = zmq.receive(obj, 1)
   zmq.free(obuf)
end

t1 = zmq.now()

dt = (t1 - t0) / 1.0e+6

print(string.format("msg size %d [B]\t test time %e [s]\t latency %e [us]", 
    bytes, dt, ((t1 - t0) / count) / 2))

zmq.free(ibuf)
zmq.destroy(obj)
