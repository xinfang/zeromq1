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

set argv [split $argv]
if {[llength $argv] < 2} {
   puts "usage: <count> <size>" 
   exit
}

set count [lindex $argv 0]
set bytes [lindex $argv 1]

load libtclzmq.so

set obj [ZMQ::create 127.0.0.1]

set eid [ZMQ::create_exchange $obj "EL" \
		$ZMQ::SCOPE_LOCAL "" $ZMQ::STYLE_DATA_DISTRIBUTION]

set qid [ZMQ::create_queue $obj "QL" $ZMQ::SCOPE_LOCAL "" \
		$ZMQ::NO_LIMIT $ZMQ::NO_LIMIT $ZMQ::NO_SWAP]

ZMQ::bind $obj "EL" "QG" "" ""
ZMQ::bind $obj "EG" "QL" "" ""

# Allocate a buffer iof the desired size
set ibuf [ZMQ::buffer $bytes]

puts [time {
   ZMQ::send $obj $eid $ibuf 1
   set obuf [ZMQ::receive $obj 1]
   ZMQ::free $obuf
} $count]

ZMQ::free $ibuf
ZMQ::destroy $obj

