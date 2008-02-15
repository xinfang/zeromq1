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

test_msg_size_start <- 1
test_msg_size_steps <- 14

#---------------------  do not edit below this line  --------------------------
test_threads <- 1
results <- data.frame ()


for (i in 0:(test_msg_size_steps - 1)) {
    msg_size <- 2^i

    file_out <- file.path (paste (msg_size, "_", test_threads - 1, "_", "out.dat", sep = ""));
    outtimes <- scan (file_out, list (0), quiet = T) [[1]]

    file_in <- file.path (paste (msg_size, "_", test_threads - 1, "_", "in.dat", sep = ""));
    intimes <- scan (file_in, list (0), quiet = T) [[1]]
    
    latencies <- (intimes - outtimes) / 2

    medlat <- median (latencies)

    print (paste ('Message size: ', msg_size))
    print (paste ('Minimum latency: ', min (latencies), ' us'))
    print (paste ('Maximum latency: ', max (latencies), ' us'))
    print (paste ('Average latency (', length (latencies) , 'msgs ): ', round (mean (latencies)), ' us'))
    print (paste ('Median latency: ', medlat, ' us'))
    print (paste ('Standard deviation of latency: ', round (sd (latencies)), ' us'))
    print (paste ('Robust deviation of latency: ', round (mean (abs (latencies - medlat))), ' us'))
    frame <- data.frame (msg_size, length (latencies), min (latencies), max (latencies), round (mean (latencies)), medlat, round (sd (latencies)), round (mean (abs (latencies - medlat))))
    results <- rbind (results, frame)
}
write.table (results, file = "latency.dat", col.names = FALSE, row.names = FALSE)

#plot(results$msg_size, results$medlat, pch=3)
