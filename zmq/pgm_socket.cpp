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

zmq::pgm_socket_t::pgm_socket_t (bool receiver_, bool passive_, 
    const char *network_, uint16_t port_, size_t readbuf_size_): g_transport (NULL), 
    pgm_msgv (NULL), pgm_msgv_len (-1)
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

    // common parameters
    pgm_transport_set_max_tpdu (g_transport, g_max_tpdu);
    pgm_transport_set_hops (g_transport, 16);

    // receiver transport
    if (receiver_) {
  
        // Set  transport->may_close_on_failure to true
        // after data los recvmsgv returns -1 errno set to ECONNRESET
        pgm_transport_set_close_on_failure (g_transport);

        // Set transport->can_send_data = FALSE, transport->can_send_nak !passive_
        pgm_transport_set_recv_only (g_transport, passive_);

        // Set NAK transmit back-off interval [us] 
        pgm_transport_set_nak_bo_ivl (g_transport, 50*1000);
    
        // Set timeout before repeating NAK [us]
        pgm_transport_set_nak_rpt_ivl (g_transport, 200*1000);

        // Set timeout for receiving RDATA
        pgm_transport_set_nak_rdata_ivl (g_transport, 200*1000);

        // Set retries for NAK without NCF/DATA
        pgm_transport_set_nak_data_retries (g_transport, 2);

        // Set retries for NCF after NAK (NAK_NCF_RETRIES)
        pgm_transport_set_nak_ncf_retries (g_transport, 2);

        pgm_transport_set_peer_expiry (g_transport, 5*8192*1000);
	    pgm_transport_set_spmr_expiry (g_transport, 250*1000);

        pgm_transport_set_rxw_sqns (g_transport, g_sqns);
        
    // sender transport
    } else {
        // Set transport->can_recv = FALSE
        pgm_transport_set_send_only (g_transport);

        pgm_transport_set_txw_max_rte (g_transport, g_max_rte);

        // preallocate full (g_sqns) window
        pgm_transport_set_txw_preallocate (g_transport, g_sqns);

	    pgm_transport_set_txw_sqns (g_transport, g_sqns);

        pgm_transport_set_ambient_spm (g_transport, 8192*1000);

        guint spm_heartbeat[] = { 1*1000, 1*1000, 2*1000, 4*1000, 8*1000, 16*1000, 
            32*1000, 64*1000, 128*1000, 256*1000, 512*1000, 1024*1000, 2048*1000, 4096*1000, 8192*1000 };
	    pgm_transport_set_heartbeat_spm (g_transport, spm_heartbeat, G_N_ELEMENTS(spm_heartbeat));
    }


    rc = pgm_transport_bind (g_transport);
    assert (rc == 0);

    if (receiver_) {
        // allocate pgm_msgv array
        pgm_msgv_len = get_max_apdu_at_once (readbuf_size_);
        pgm_msgv = new pgm_msgv_t [pgm_msgv_len];
    }

    printf ("TSI: %s\n", pgm_print_tsi (&g_transport->tsi));
}

zmq::pgm_socket_t::~pgm_socket_t ()
{
    if (pgm_msgv) {
        delete [] pgm_msgv;
    }

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

// send one PGM data packet, transmit window owned memory.
size_t zmq::pgm_socket_t::write_one_pkt (unsigned char *data_, size_t data_len_)
{
    iovec iov = {data_,data_len_};

    ssize_t nbytes = pgm_transport_send_packetv (g_transport, &iov, 1, MSG_DONTWAIT | MSG_WAITALL, true);

    assert (nbytes != -EINVAL);

    if (nbytes == -1 && errno != EAGAIN) {
        errno_assert (0);
    }

    nbytes = nbytes == -1 ? 0 : nbytes;

    printf ("wrote %iB, %s(%i)\n", (int)nbytes, __FILE__, __LINE__);
    
    // We have to write all data as one packet
    if (nbytes > 0) {
        assert (nbytes = data_len_);
    }

    return nbytes;
}

size_t zmq::pgm_socket_t::get_max_tsdu (bool can_fragment_)
{
    return (size_t)pgm_transport_max_tsdu (g_transport, can_fragment_);
}

size_t zmq::pgm_socket_t::get_max_apdu_at_once (size_t readbuf_size_)
{
    assert (readbuf_size_ > 0);

    size_t max_apdus = (int)readbuf_size_ / get_max_tsdu (false);

    printf ("maxapdu_at_once %i, max tsdu %i\n", (int)max_apdus, (int)get_max_tsdu (false));

    // should have at least one apdu per read
    assert (max_apdus);

    return max_apdus;
}

unsigned char *zmq::pgm_socket_t::alloc_one_pkt (bool can_fragment_)
{
    // Slice for packet of the size returned with max_tsdu (can_fragment_)
    return (unsigned char*)pgm_packetv_alloc (g_transport, can_fragment_);
}

void zmq::pgm_socket_t::free_one_pkt (unsigned char *data_, bool can_fragment_)
{
    // Push allocated slice into the trash_data
    pgm_packetv_free1 (g_transport, data_, can_fragment_);
}

void zmq::pgm_socket_t::drop_superuser ()
{
    pgm_drop_superuser ();
}

/*
char zmq::pgm_socket_t::read_one_byte_from_waiting_pipe ()
{
    char buf;
    while (1 == read (g_transport->waiting_pipe[0], &buf, sizeof(buf)));
    printf ("read 1B \'%c\' from waititng pipe\n", buf);
    return buf;
}
*/

size_t zmq::pgm_socket_t::read_one_pkt (iovec *iov_)
{
    assert (0);
}

size_t zmq::pgm_socket_t::read_pkt (iovec *iov_, size_t iov_len_)
{

    assert (iov_len_ == pgm_msgv_len);

    int translated = 0;

    // jusr for debug to see how many pgm_msgv_t we have translated
    size_t msgv_read = 0;
    
    iovec *iov = iov_;
    iovec *iov_end = iov + iov_len_;

    ssize_t nbytes = pgm_transport_recvmsgv (g_transport, pgm_msgv, pgm_msgv_len, MSG_DONTWAIT);
   
    pgm_msgv_t *msgv = pgm_msgv;

    printf ("raw received %i bytes", (int)nbytes);
    if (nbytes < 0) { 
        printf (", errno %i %s", errno, strerror (errno));
    }
    printf (", %s(%i)\n", __FILE__, __LINE__);

    // In a case when not ODATA/DRATA fired POLLIN event
    // pgm_transport_recvmsg returns -1 with  errno == EAGAIN
    // 
    // nbytes == -1 errno == ECONNRESET for data loss
    if (nbytes == -1 && errno != EAGAIN) {
        fflush (stdout);
        errno_assert (0); 
    }

    // tanslate from pgm_msgv_t into iovec
    while (translated < (int)nbytes) {
        assert (iov <= iov_end);

        iov->iov_base = (msgv->msgv_iov)->iov_base;
        iov->iov_len = (msgv->msgv_iov)->iov_len;
        
        // only one apdu per pgm_msgv_t structure
        assert (msgv->msgv_iovlen == 1);
        
        translated += iov->iov_len;

        msgv++;
        msgv_read++;
        iov++;

        printf ("translated %i, nbytes %i\n", translated, (int)nbytes);
    }

    nbytes = nbytes == -1 ? 0 : nbytes;
    printf ("received %i bytes, ", (int)nbytes);
    printf ("in %i pgm_msgv_t, %s(%i)\n", msgv_read, __FILE__, __LINE__);
    fflush (stdout); 

    assert (nbytes == translated);

    return (size_t)nbytes;
}

