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

#  Read the input data and compute cycles
times <- scan ('in.dat', list (0), quiet = T) [[1]]
cycles <- diff (times)

#  Save the cycle times
write (cycles, file = 'density.dat', sep = '\n')

#  Compute and save the aggregates
medoh <- median (cycles)
mth <- length (times) * 1000000 / (max (times) - min (times))

write (c (
    paste ('Minimum density: ', min (cycles), ' us/msg'),
    paste ('Maximum density: ', max (cycles), ' us/msg'),
    paste ('Average density: ', round (mean (cycles), digits = 3), ' us/msg'),
    paste ('Median density: ', medoh, ' us/msg'),
    paste ('Standard deviation of density: ', round (sd (cycles), digits = 3), ' us/msg'),
    paste ('Robust deviation of density: ', round (mean (abs (cycles - medoh)), digits = 3), ' us/msg'),
    paste ('Mean throughput (', length (times), 'msgs ) : ', round (mth), 'msg/sec')
    ), file = 'density_agg.txt', sep = '\n')

