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

#include "pgm_socket.hpp"
#include "err.hpp"

zmq::pgm_socket_t::pgm_socket_t (bool receiver_, bool pasive_, 
    const char *network_, uint16_t port_): g_transport (NULL) 
//    , received_count (0), send_nak_each (3)
{
    printf ("GLIB: %i.%i.%i\n", GLIB_MAJOR_VERSION, GLIB_MINOR_VERSION, GLIB_MICRO_VERSION);

    pgm_init ();

    pgm_gsi_t gsi;

    int rc = pgm_create_md5_gsi (&gsi);
	assert (rc == 0);

    struct pgm_sock_mreq recv_smr, send_smr;
    int smr_len = 1;
	rc = pgm_if_parse_transport (network_, AF_INET, &recv_smr, &send_smr, &smr_len);
    assert (rc == 0);
	assert (smr_len == 1);

    rc = pgm_transport_create (&g_transport, &gsi, port_, &recv_smr, 1, &send_smr);
    errno_assert (rc == 0);

    int g_sqns = 10;
    int g_max_rte = 400*1000;
    int g_max_tpdu = 1500;

    if (!receiver_) {
//        g_sqns = 2;
//        g_max_rte = 400*1000;
    }

    pgm_transport_set_max_tpdu (g_transport, g_max_tpdu);
	pgm_transport_set_txw_sqns (g_transport, g_sqns);
    pgm_transport_set_rxw_sqns (g_transport, g_sqns);
    pgm_transport_set_hops (g_transport, 16);
	pgm_transport_set_ambient_spm (g_transport, 8192*1000);
    guint spm_heartbeat[] = { 1*1000, 1*1000, 2*1000, 4*1000, 8*1000, 16*1000, 
        32*1000, 64*1000, 128*1000, 256*1000, 512*1000, 1024*1000, 2048*1000, 4096*1000, 8192*1000 };
	pgm_transport_set_heartbeat_spm (g_transport, spm_heartbeat, G_N_ELEMENTS(spm_heartbeat));
    pgm_transport_set_peer_expiry (g_transport, 5*8192*1000);
	pgm_transport_set_spmr_expiry (g_transport, 250*1000);


    if (receiver_) {
        // Set NAK transmit back-off interval [us] 
        pgm_transport_set_nak_bo_ivl (g_transport, 50*1000);
        // Set timeout before repeating NAK [us]
        pgm_transport_set_nak_rpt_ivl (g_transport, 200*1000);
        // Set timeout for receiving RDATA
        pgm_transport_set_nak_rdata_ivl (g_transport, 200*1000);
        // Set retries for DATA packets after NAK
        pgm_transport_set_nak_data_retries (g_transport, 2);
        // Set retries for DATA after NCF
        pgm_transport_set_nak_ncf_retries (g_transport, 2);

        pgm_transport_set_recv_only (g_transport, FALSE);

    } else {
        pgm_transport_set_txw_max_rte (g_transport, g_max_rte);

        pgm_transport_set_send_only (g_transport);

    }

    rc = pgm_transport_bind (g_transport);
    assert (rc == 0);

    printf ("TSI: %s\n", pgm_print_tsi (&g_transport->tsi));
}

zmq::pgm_socket_t::~pgm_socket_t ()
{
    pgm_transport_destroy (g_transport, TRUE);
}


int zmq::pgm_socket_t::get_fd_count (short events_)
{
    int nfds = IP_MAX_MEMBERSHIPS;
    pollfd fds [IP_MAX_MEMBERSHIPS];
    memset (fds, '\0', sizeof (fds));

    int rc = pgm_transport_poll_info (g_transport, (pollfd*)&fds, &nfds, events_);
//    printf ("nfds %i, %s(%i)\n", rc, __FILE__, __LINE__);
    assert (rc <= nfds);

    return nfds;
}

int zmq::pgm_socket_t::get_pfds (pollfd *fds_, int count_, short events_)
{
    int rc = pgm_transport_poll_info (g_transport, fds_, &count_, events_);
    assert (rc == count_);

    return rc;
}

/*
size_t zmq::pgm_socket_t::write (unsigned char *data_, size_t size_)
{
    ssize_t nbytes = pgm_transport_send (g_transport, data_, size_, 0);
    errno_assert (nbytes != -1);
    return (size_t) nbytes;
}
*/

size_t zmq::pgm_socket_t::write_pkt (const struct iovec *iovec_, int niovecs_)
{
//    ssize_t nbytes = pgm_transport_sendv3_pkt_dontwait (g_transport, iovec_, niovecs_, MSG_DONTWAIT);
 
    ssize_t nbytes = pgm_transport_send_packetv (g_transport, iovec_, niovecs_, MSG_DONTWAIT, true);
    // In case nbytes == -1 and errno == EAGAIN have to try again
    // otherwise error
    if (nbytes == -1 && errno != EAGAIN) {
        errno_assert (0);
    }

    nbytes = nbytes == -1 ? 0 : nbytes;

    printf ("wrote %iB, %s(%i)\n", (int)nbytes, __FILE__, __LINE__);

    return nbytes;
}

/*
size_t zmq::pgm_socket_t::read (unsigned char *data_, size_t size_)
{ 
    ssize_t nbytes = pgm_transport_recv (g_transport, data_, size_, 0);
    errno_assert (nbytes != -1);
    return (size_t) nbytes;
}
*/

size_t zmq::pgm_socket_t::read_msg (iovec **iov_)
{
//    printf ("%s(%i)\n", __FILE__, __LINE__);
    ssize_t nbytes = pgm_transport_recvmsgv (g_transport, &msgv, 1, MSG_DONTWAIT);

    // In a case when not ODATA fired POLLIN event
    // pgm_transport_recvmsg returns -1 with  errno == EAGAIN
    if (nbytes == -1 && errno != EAGAIN) {
        printf ("errno %i, ", errno);
        errno_assert (nbytes != -1); 
    }

    nbytes = nbytes == -1 ? 0 : nbytes;
    printf ("received %i bytes, ", (int)nbytes);
    printf ("in %i iovecs, %s(%i)\n", nbytes == 0 ? 0 : (int)msgv.msgv_iovlen, __FILE__, __LINE__);
//    fflush (stdout); 

    // iov
    *iov_ = msgv.msgv_iov;

/*
    // NAK testing
    int rc;
    if (nbytes != 0) {
        if (received_count > 0 && send_nak_each > 0) {
            if (received_count % send_nak_each == 0) {
                rc = send_nak (received_count);
                assert (rc != -1);
            }
        }
        received_count++;
    }
*/

    return (size_t)nbytes;
}

/*
int zmq::pgm_socket_t::send_nak (int seq_num_)
{
    if (!g_transport->peers_list) {
		return -1;
	}

    GList* list = g_transport->peers_list;
	do {
		GList* next = list->next;
		pgm_peer_t* peer = (pgm_peer_t*)list->data;
        ::send_nak (peer, seq_num_);

        printf ("TSI: %s\n", pgm_print_tsi (&(peer->tsi)));
        list = next;
    } while (list);

    return 0;
}
*/
