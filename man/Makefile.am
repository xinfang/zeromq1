dist_man_MANS = man1/zmq_server.1 man1/estimate_cpu_freq.1 man7/zmq.7 \
man7/zmq-latency.7 man7/zmq-perf.7 man7/zmq-tests.7 man7/zmq-tests-build.7 \
man7/zmq-throughput.7 man7/zmq-transport.7 man3/zmq-c-api.3 man1/local_lat.1 \
man1/local_thr.1 man1/remote_lat.1 man1/remote_thr.1

install-data-hook:
		$(mkdir_p) "$(DESTDIR)$(mandir)/man3";
		$(INSTALL_DATA) "$(top_srcdir)/man/man3/zmq__api_thread_t.3"\
			"$(DESTDIR)$(mandir)/man3/zmq::api_thread_t.3";
		$(INSTALL_DATA) "$(top_srcdir)/man/man3/zmq__dispatcher_t.3"\
			"$(DESTDIR)$(mandir)/man3/zmq::dispatcher_t.3";
		$(INSTALL_DATA) "$(top_srcdir)/man/man3/zmq__locator_t.3"\
			"$(DESTDIR)$(mandir)/man3/zmq::locator_t.3";
		$(INSTALL_DATA) "$(top_srcdir)/man/man3/zmq__message_t.3"\
			"$(DESTDIR)$(mandir)/man3/zmq::message_t.3";
		$(INSTALL_DATA) "$(top_srcdir)/man/man3/zmq__io_thread_t.3"\
			"$(DESTDIR)$(mandir)/man3/zmq::io_thread_t.3";
		$(INSTALL_DATA) "$(top_srcdir)/man/man3/zmq__select_thread_t.3"\
			"$(DESTDIR)$(mandir)/man3/zmq::select_thread_t.3";
		$(INSTALL_DATA) "$(top_srcdir)/man/man3/zmq__poll_thread_t.3"\
			"$(DESTDIR)$(mandir)/man3/zmq::poll_thread_t.3";
		$(INSTALL_DATA) "$(top_srcdir)/man/man3/zmq__epoll_thread_t.3"\
			"$(DESTDIR)$(mandir)/man3/zmq::epoll_thread_t.3";
		$(INSTALL_DATA) "$(top_srcdir)/man/man3/zmq__devpoll_thread_t.3"\
			"$(DESTDIR)$(mandir)/man3/zmq::devpoll_thread_t.3";
		$(INSTALL_DATA) "$(top_srcdir)/man/man3/zmq__kqueue_thread_t.3"\
			"$(DESTDIR)$(mandir)/man3/zmq::kqueue_thread_t.3";
		$(INSTALL_DATA) "$(top_srcdir)/man/man3/zmq__set_error_handler.3"\
			"$(DESTDIR)$(mandir)/man3/zmq::set_error_handler.3";
		$(INSTALL_DATA) "$(top_srcdir)/man/man3/zmq__zmq_create.3"\
			"$(DESTDIR)$(mandir)/man3/zmq::zmq_create.3";
		$(INSTALL_DATA) "$(top_srcdir)/man/man3/zmq__zmq_destroy.3"\
			"$(DESTDIR)$(mandir)/man3/zmq::zmq_destroy.3";
		$(INSTALL_DATA) "$(top_srcdir)/man/man3/zmq__zmq_mask.3"\
			"$(DESTDIR)$(mandir)/man3/zmq::zmq_mask.3";
		$(INSTALL_DATA) "$(top_srcdir)/man/man3/zmq__zmq_create_exchange.3"\
			"$(DESTDIR)$(mandir)/man3/zmq::zmq_create_exchange.3";
		$(INSTALL_DATA) "$(top_srcdir)/man/man3/zmq__zmq_create_queue.3"\
			"$(DESTDIR)$(mandir)/man3/zmq::zmq_create_queue.3";
		$(INSTALL_DATA) "$(top_srcdir)/man/man3/zmq__zmq_bind.3"\
			"$(DESTDIR)$(mandir)/man3/zmq::zmq_bind.3";
		$(INSTALL_DATA) "$(top_srcdir)/man/man3/zmq__zmq_send.3"\
			"$(DESTDIR)$(mandir)/man3/zmq::zmq_send.3";
		$(INSTALL_DATA) "$(top_srcdir)/man/man3/zmq__zmq_receive.3"\
			"$(DESTDIR)$(mandir)/man3/zmq::zmq_receive.3";
		$(INSTALL_DATA) "$(top_srcdir)/man/man3/zmq__zmq_free.3"\
			"$(DESTDIR)$(mandir)/man3/zmq::zmq_free.3";

distclean-local:
		-rm  *.pdf
		-rm  man1/*.ps
		-rm  man3/*.ps
		-rm  man7/*.ps

dist-hook:
		./convert2pdf.sh
		$(mkdir_p) $(top_distdir)/doc
		cp $(top_srcdir)/man/*.pdf $(top_distdir)/doc

