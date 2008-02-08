#
#    Copyright (c) 2007-2008 FastMQ Inc.
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

#  Read the input files and compute latencies
intimes <- scan ('in.dat', list (0), quiet = T) [[1]]
outtimes <- scan ('out.dat', list (0), quiet = T) [[1]]
latencies <- (intimes - outtimes) / 2

#  Save the latencies
write (latencies, file = 'latency.dat', sep = '\n')

#  Compute and save the aggregates
medlat <- median (latencies)
write (c (
    paste ('Minimum latency: ', min (latencies), ' us'),
    paste ('Maximum latency: ', max (latencies), ' us'),
    paste ('Average latency (', length (latencies) , 'msgs ): ', round (mean (latencies)), ' us'),
    paste ('Median latency: ', medlat, ' us'),
    paste ('Standard deviation of latency: ', round (sd (latencies)), ' us'),
    paste ('Robust deviation of latency: ', round (mean (abs (latencies - medlat))), ' us')
    ), file = 'latency_agg.txt', sep = '\n')
