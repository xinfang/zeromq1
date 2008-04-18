#include <cstdio>
#include <glib.h>
#include <unistd.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>

struct transport_t
{
    GThread *timer_thread;
    GMainLoop *timer_loop;
    GMainContext *timer_context;
    int timer_pipe[2];
    GIOChannel *timer_channel;
};


static gpointer
timer_thread (
	gpointer		data_
	)
{
    transport_t *transport = (transport_t*)data_;
    transport->timer_context = g_main_context_new ();
    transport->timer_loop = g_main_loop_new (transport->timer_context, FALSE);
	g_debug ("timer_thread entering event loop.");
	g_main_loop_run (transport->timer_loop);
	g_debug ("timer_thread leaving event loop.");

    g_main_loop_unref (transport->timer_loop);

    return NULL;
}

int set_nonblocking (int filedes [2]);


int main ()
{
    g_thread_init (NULL);


    transport_t transport;

    
	GThread *thread = g_thread_create_full (timer_thread,
					&transport,
					0,
					TRUE,
					TRUE,
					G_THREAD_PRIORITY_HIGH,
					NULL);
	if (thread) {
		transport.timer_thread = thread;
	} else {
        assert (0);
	}
    
    sleep (1);

    g_debug ("create tx to timer pipe.");
	int retval = pipe (transport.timer_pipe);
	if (retval < 0) {
        assert (0);
	}

    retval = set_nonblocking (transport.timer_pipe);
	if (retval < 0) {
		assert (0);
	}

    transport.timer_channel = g_io_channel_unix_new (transport.timer_pipe[0]);
//	g_io_add_watch_context_full (transport.timer_channel, transport.timer_context, G_PRIORITY_HIGH, G_IO_IN, on_timer_pipe, transport, NULL);

    sleep (5);

    g_main_loop_quit (transport.timer_loop);
    g_thread_join (thread);
	transport.timer_thread = NULL;

 	if (transport.timer_pipe[0]) {
		close (transport.timer_pipe[0]);
		transport.timer_pipe[0] = 0;
	}
	if (transport.timer_pipe[1]) {
		close (transport.timer_pipe[1]);
		transport.timer_pipe[1] = 0;
	}
   
    return 0;
}

int set_nonblocking (int filedes[2])
{
	int retval = 0;

/* set write end non-blocking */
	int fd_flags = fcntl (filedes[1], F_GETFL);
	if (fd_flags < 0) {
		retval = fd_flags;
		goto out;
	}
	retval = fcntl (filedes[1], F_SETFL, fd_flags | O_NONBLOCK);
	if (retval < 0) {
		retval = fd_flags;
		goto out;
	}
/* set read end non-blocking */
	fcntl (filedes[0], F_GETFL);
	if (fd_flags < 0) {
		retval = fd_flags;
		goto out;
	}
	retval = fcntl (filedes[0], F_SETFL, fd_flags | O_NONBLOCK);
	if (retval < 0) {
		retval = fd_flags;
		goto out;
	}

out:
	return retval;
}

