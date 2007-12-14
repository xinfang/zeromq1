#
#    Copyright (c) 2007 FastMQ Inc.
#
#    This file is part of 0MQ.
#
#    0MQ is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 3 of the License, or
#    (at your option) any later version.
#
#    0MQ is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

#number of messages in test
msgs <- 50000

#  Read the input data
intimes <- scan ('in.dat', list (0)) [[1]]
outtimes <- scan ('out.dat', list (0)) [[1]]

#  Compute and save the aggregates
time_stop <- intimes [1]
time_start <- outtimes [1]

write (c (
    paste ('Average latency (', msgs , 'msgs ): ', round ((time_stop - time_start) / msgs ) / 2, ' us')
    ), file = 'latency_agg.txt', sep = '\n')

