#!/bin/sh
#
# Copyright (c) 2007-2009 FastMQ Inc.
#
# This file is part of 0MQ.
#
# 0MQ is free software; you can redistribute it and/or modify it under
# the terms of the Lesser GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# 0MQ is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# Lesser GNU General Public License for more details.
#
# You should have received a copy of the Lesser GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

groff -man -Tps man1/zmq_server.1 > man1/zmq_server.1.ps
ps2pdf man1/zmq_server.1.ps zmq_server.pdf

groff -man -Tps man1/estimate_cpu_freq.1 > man1/estimate_cpu_freq.1.ps
ps2pdf man1/estimate_cpu_freq.1.ps estimate_cpu_freq.pdf

groff -man -Tps man3/zmq__api_thread_t.3 > man3/zmq__api_thread_t.3.ps
ps2pdf man3/zmq__api_thread_t.3.ps api_thread_t.pdf

groff -man -Tps man3/zmq__dispatcher_t.3 > man3/zmq__dispatcher_t.3.ps
ps2pdf man3/zmq__dispatcher_t.3.ps dispatcher_t.pdf

groff -man -Tps man3/zmq__io_thread_t.3 > man3/zmq__io_thread_t.3.ps
ps2pdf man3/zmq__io_thread_t.3.ps io_thread_t.pdf

groff -man -Tps man3/zmq__locator_t.3 > man3/zmq__locator_t.3.ps
ps2pdf man3/zmq__locator_t.3.ps locator_t.pdf

groff -man -Tps man3/zmq__poll_thread_t.3 > man3/zmq__poll_thread_t.3.ps
ps2pdf man3/zmq__poll_thread_t.3.ps poll_thread_t.pdf

groff -man -Tps man3/zmq__set_error_handler.3 > man3/zmq__set_error_handler.3.ps
ps2pdf man3/zmq__set_error_handler.3.ps set_error_handler.pdf

groff -man -Tps man3/zmq__devpoll_thread_t.3 > man3/zmq__devpoll_thread_t.3.ps
ps2pdf man3/zmq__devpoll_thread_t.3.ps devpoll_thread_t.pdf

groff -man -Tps man3/zmq__epoll_thread_t.3 > man3/zmq__epoll_thread_t.3.ps
ps2pdf man3/zmq__epoll_thread_t.3.ps epoll_thread_t.pdf

groff -man -Tps man3/zmq__kqueue_thread_t.3 > man3/zmq__kqueue_thread_t.3.ps
ps2pdf man3/zmq__kqueue_thread_t.3.ps kqueue_thread_t.pdf

groff -man -Tps man3/zmq__message_t.3 > man3/zmq__message_t.3.ps
ps2pdf man3/zmq__message_t.3.ps message_t.pdf

groff -man -Tps man3/zmq__select_thread_t.3 > man3/zmq__select_thread_t.3.ps
ps2pdf man3/zmq__select_thread_t.3.ps select_thread_t.pdf

groff -man -Tps man3/zmq-c-api.3 > man3/zmq-c-api.3.ps
ps2pdf man3/zmq-c-api.3.ps zmq-c-api.pdf

groff -man -Tps man7/zmq.7 > man7/zmq.7.ps
ps2pdf man7/zmq.7.ps zmq.pdf

groff -man -Tps man7/zmq-latency.7 > man7/zmq-latency.7.ps
ps2pdf man7/zmq-latency.7.ps zmq-latency.pdf

groff -man -Tps man7/zmq-perf.7 > man7/zmq-perf.7.ps
ps2pdf man7/zmq-perf.7.ps zmq-perf.pdf

groff -man -Tps man7/zmq-tests.7 > man7/zmq-tests.7.ps
ps2pdf man7/zmq-tests.7.ps zmq-tests.pdf

groff -man -Tps man7/zmq-tests-build.7 > man7/zmq-tests-build.7.ps
ps2pdf man7/zmq-tests-build.7.ps zmq-tests-build.pdf

groff -man -Tps man7/zmq-throughput.7 > man7/zmq-throughput.7.ps
ps2pdf man7/zmq-throughput.7.ps zmq-throughput.pdf

groff -man -Tps man7/zmq-transport.7 > man7/zmq-transport.7.ps
ps2pdf man7/zmq-transport.7.ps zmq-transport.pdf

