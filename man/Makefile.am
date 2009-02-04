dist_man_MANS = man1/zmq_server.1 man7/zmq.7

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

