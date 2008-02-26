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

#  Read the input files
data <- read.table ("in.dat")

thrput_msg <- (1000000 * data$V2) / (data$V4 - data$V3)
thrput_bit <- (thrput_msg * 8 * data$V1) / 1000000

thrput_msg <- round (thrput_msg, digits = 1)
thrput_bit <- round (thrput_bit, digits = 1)

#  Save the results
data_frame <- data.frame (data$V1, data$V2, thrput_msg, thrput_bit)
write.table (data_frame, file = "thrput.dat", col.names = FALSE, row.names = FALSE)

