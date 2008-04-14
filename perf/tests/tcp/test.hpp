/*
    Copyright (c) 2007-2008 FastMQ Inc.

    This file is part of 0MQ.

    0MQ is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    0MQ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define TEST_MSG_SIZE_START 1
#define TEST_MSG_SIZE_STEPS 15
#define TEST_TIME 5000

// MT test
#define TEST_THREADS 1

// latency test msg count denominator
#define SYS_LAT_DEN 100

// slope for messages smaller than SYS_BREAK
// 100 000 msgs
#define SYS_SLOPE 0.55
#define SYS_OFF 110

#define SYS_BREAK 1024

// slope for messages bigger than SYS_BREAK
// 100 000
#define SYS_SLOPE_BIG 0.89
#define SYS_OFF_BIG -80

#define PORT_NUMBER 5555

struct worker_args_t
{
    int id;
    int msg_size;
    int msg_count;
};
