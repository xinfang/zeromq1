/*
    Copyright (c) 2007-2009 FastMQ Inc.

    This file is part of 0MQ.

    0MQ is free software; you can redistribute it and/or modify it under
    the terms of the Lesser GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    0MQ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    Lesser GNU General Public License for more details.

    You should have received a copy of the Lesser GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <zmq/platform.hpp>

#if (defined ZMQ_HAVE_OPENPGM && defined ZMQ_HAVE_LINUX) \
    || defined ZMQ_HAVE_WINDOWS

#ifdef ZMQ_HAVE_LINUX
#include <pgm/pgm.h>
#else 
#include <Winsock2.h>
#include <Wsrm.h>
#include <ws2spi.h>
#endif

#include <zmq/err.hpp>
#include <string>
#include <assert.h>
#include <zmq/config.hpp>
#include <iostream>

#include <zmq/pgm_socket.hpp>

//#define PGM_SOCKET_DEBUG
//#define PGM_SOCKET_DEBUG_LEVEL 1

// level 1 = key behaviour
// level 2 = processing flow
// level 4 = infos

#ifndef PGM_SOCKET_DEBUG
#   define zmq_log(n, ...)  while (0)
#else
#   define zmq_log(n, ...)    do { if ((n) <= PGM_SOCKET_DEBUG_LEVEL) \
        { printf (__VA_ARGS__);}} while (0)
#endif

#ifdef ZMQ_HAVE_LINUX

zmq::pgm_socket_t::pgm_socket_t (bool receiver_, const char *interface_, 
      size_t readbuf_size_) : 
    g_transport (NULL), 
    receiver (receiver_),
    port_number (0),
    udp_encapsulation (false),
    readbuf_size (readbuf_size_),
    pgm_msgv (NULL),
    nbytes_rec (0),
    nbytes_processed (0),
    pgm_msgv_processed (0),
    pgm_msgv_len (0)
{
    
    //  Check if we are encapsulating into UDP, interface string has to 
    //  start with udp:.

    const char *interface_ptr = interface_;

    if (strlen (interface_) > 4= && interface_ [0] == 'u' && 
          interface_ [1] == 'd' && interface_ [2] == 'p' && 
          interface_ [3] == ':') {
    
        //  Shift interface_ptr after ':'.
        interface_ptr += 4;

        udp_encapsulation = true;
    }
 
    //  Parse port number.
    char *port_delim = strchr (interface_ptr, ':');
    assert (port_delim);

    port_number = atoi (port_delim + 1);
  
    //  Store interface string.
    assert (port_delim > interface_ptr);
    assert (port_delim - interface_ptr < (int) sizeof (network) - 1);

    memset (network, '\0', sizeof (network));
    memcpy (network, interface_ptr, port_delim - interface_ptr);

    zmq_log (1, "parsed: network  %s, port %i, udp encaps. %s, %s(%i)\n", 
        network, port_number, udp_encapsulation ? "yes" : "no",
        __FILE__, __LINE__);

    //  Open PGM transport.
    open_transport ();

    //  For receiver transport preallocate pgm_msgv array.
    if (receiver_) {
        pgm_msgv_len = get_max_apdu_at_once (readbuf_size_);
        pgm_msgv = new pgm_msgv_t [pgm_msgv_len];
    }

}

void zmq::pgm_socket_t::open_transport (void)
{

    zmq_log (1, "Opening PGM: network  %s, port %i, udp encaps. %s, %s(%i)\n",
        network, port_number, udp_encapsulation ? "yes" : "no", 
        __FILE__, __LINE__);

    //  Can not open transport before destroying old one. 
    assert (g_transport == NULL);

    //  Set actual_tsi and prev_tsi to zeros.
    memset (&tsi, '\0', sizeof (pgm_tsi_t));
    memset (&retired_tsi, '\0', sizeof (pgm_tsi_t));

    //  Zero counter used in msgrecv.
    nbytes_rec = 0;
    nbytes_processed = 0;
    pgm_msgv_processed = 0;

    //  Init PGM transport.
    //  Note that if you want to use gettimeofday and sleep for openPGM timing,
    //  set environment variables PGM_TIMER to "GTOD" 
    //  and PGM_SLEEP to "USLEEP".
    pgm_init ();

    pgm_gsi_t gsi;

    int rc = pgm_create_md5_gsi (&gsi);
	assert (rc == 0);

    struct group_source_req recv_gsr, send_gsr;
    gsize recv_len = 1;
  
    rc = pgm_if_parse_transport (network, AF_INET, &recv_gsr, 
        &recv_len, &send_gsr);
    assert (rc == 0);
    assert (recv_len == 1);

    //  If we are using UDP encapsulation update send_gsr & recv_gsr 
    //  structures. Note that send_gsr & recv_gsr has to be updated after 
    //  pgm_if_parse_transport call.
    if (udp_encapsulation) {

        //  Use the same port for UDP encapsulation.
        ((struct sockaddr_in*)&send_gsr.gsr_group)->sin_port = 
            g_htons (port_number);
	((struct sockaddr_in*)&recv_gsr.gsr_group)->sin_port = 
            g_htons (port_number);
    }

    rc = pgm_transport_create (&g_transport, &gsi, 0, port_number, &recv_gsr, 
        1, &send_gsr);
    errno_assert (rc == 0);

    //  Common parameters for receiver and sender.

    //  Set maximum transport protocol data unit size (TPDU).
    rc = pgm_transport_set_max_tpdu (g_transport, pgm_max_tpdu);
    assert (rc == 0);

    //  Set maximum number of network hops to cross.
    rc = pgm_transport_set_hops (g_transport, 16);
    assert (rc == 0);

    //  Receiver transport.
    if (receiver) {
  
        //  Set transport->may_close_on_failure to true,
        //  after data los recvmsgv returns -1 errno set to ECONNRESET.
        rc = pgm_transport_set_close_on_failure (g_transport, TRUE);
        assert (rc == 0);

        //  Set transport->can_send_data = FALSE.
        //  Note that NAKs are still generated by the transport.
        rc = pgm_transport_set_recv_only (g_transport, false);
        assert (rc == 0);

        //  Set NAK transmit back-off interval [us].
        rc = pgm_transport_set_nak_bo_ivl (g_transport, 50*1000);
        assert (rc ==0);
    
        //  Set timeout before repeating NAK [us].
        rc = pgm_transport_set_nak_rpt_ivl (g_transport, 200*1000);
        assert (rc == 0);

        //  Set timeout for receiving RDATA.
        rc = pgm_transport_set_nak_rdata_ivl (g_transport, 200*1000);
        assert (rc == 0);

        //  Set retries for NAK without NCF/DATA (NAK_DATA_RETRIES).
        rc = pgm_transport_set_nak_data_retries (g_transport, 5);
        assert (rc == 0);

        //  Set retries for NCF after NAK (NAK_NCF_RETRIES).
        rc = pgm_transport_set_nak_ncf_retries (g_transport, 2);
        assert (rc == 0);

        //  Set timeout for removing a dead peer [us].
        rc = pgm_transport_set_peer_expiry (g_transport, 5*8192*1000);
        assert (rc == 0);

        //  Set expiration time of SPM Requests [us].
        rc = pgm_transport_set_spmr_expiry (g_transport, 25*1000);
        assert (rc == 0);

        if (pgm_window_size > 0) {
            //  Set receive window size in sequence numbers.
            rc = pgm_transport_set_rxw_sqns (g_transport, pgm_window_size);
            assert (rc == 0);

        } else {

            //  Set the size of the receive window size by max
            //  data rate in bytes per second.
            assert (pgm_max_rte > 0);
            rc = pgm_transport_set_rxw_max_rte (g_transport, pgm_max_rte);
            assert (rc ==0);

            //  Set receive window size in seconds. 
            assert (pgm_secs > 0);
            rc = pgm_transport_set_rxw_secs (g_transport, pgm_secs);
            assert (rc == 0);
        }

    //  Sender transport.
    } else {

        //  Set transport->can_recv = FALSE, waiting_pipe wont not be read.
        rc = pgm_transport_set_send_only (g_transport, TRUE);
        assert (rc == 0);

        int to_preallocate = 0;

        if (pgm_window_size > 0) {
            //  Set send window size in sequence numbers.
            rc = pgm_transport_set_txw_sqns (g_transport, pgm_window_size);
            assert (rc == 0);

            //  Preallocate full window.
            to_preallocate = pgm_window_size;

        } else {

            //  Set the size of the send window size by 
            //  data rate in bytes per second.
            assert (pgm_max_rte > 0);
            rc = pgm_transport_set_txw_max_rte (g_transport, pgm_max_rte);
            assert (rc ==0);

            //  Set send window size in seconds. 
            assert (pgm_secs > 0);
            rc = pgm_transport_set_txw_secs (g_transport, pgm_secs);
            assert (rc == 0);

            //  Preallocate full transmit window. For simplification always 
            //  worst case is used (40 bytes ipv6 header and 20 bytes UDP 
            //  encapsulation).
            to_preallocate = pgm_secs * pgm_max_rte / (pgm_max_tpdu - 40 - 20);
        }

        rc = pgm_transport_set_txw_preallocate (g_transport, to_preallocate);
        assert (rc == 0);

        zmq_log (1, "Preallocated %i slices in TX window. %s(%i)\n", 
            to_preallocate, __FILE__, __LINE__);

        //  Set interval of background SPM packets [us].
        rc = pgm_transport_set_ambient_spm (g_transport, 8192 * 1000);
        assert (rc == 0);

        //  Set intervals of data flushing SPM packets [us].
        guint spm_heartbeat[] = {4 * 1000, 4 * 1000, 8 * 1000, 16 * 1000, 
            32 * 1000, 64 * 1000, 128 * 1000, 256 * 1000, 512 * 1000, 
            1024 * 1000, 2048 * 1000, 4096 * 1000, 8192 * 1000};
        
	rc = pgm_transport_set_heartbeat_spm (g_transport, spm_heartbeat, 
            G_N_ELEMENTS(spm_heartbeat));
        assert (rc == 0);
    }
    
    //  Bind a transport to the specified network devices.
    rc = pgm_transport_bind (g_transport);
    assert (rc == 0);

}

zmq::pgm_socket_t::~pgm_socket_t ()
{
    //  Celanup.
    if (pgm_msgv) {
        delete [] pgm_msgv;
    }

    close_transport ();
}

void zmq::pgm_socket_t::close_transport (void)
{   
    //  g_transport has to be valid.
    assert (g_transport);

    pgm_transport_destroy (g_transport, TRUE);

    g_transport = NULL;
}

//   Get receiver fds. recv_fd is from transport->recv_sock
//   waiting_pipe_fd is from transport->waiting_pipe [0]
int zmq::pgm_socket_t::get_receiver_fds (int *recv_fd_, 
    int *waiting_pipe_fd_)
{

    //  For POLLIN there are 2 pollfds in pgm_transport.
    int fds_array_size = pgm_receiver_fd_count;
    pollfd *fds = new pollfd [fds_array_size];
    memset (fds, '\0', fds_array_size * sizeof (fds));

    //  Retrieve pollfds from pgm_transport.
    int rc = pgm_transport_poll_info (g_transport, fds, &fds_array_size, 
        POLLIN);

    //  pgm_transport_poll_info has to return 2 pollfds for POLLIN. 
    //  Note that fds_array_size parameter can be 
    //  changed inside pgm_transport_poll_info call.
    assert (rc == pgm_receiver_fd_count);
 
    //  Store pfds into user allocated space.
    *recv_fd_ = fds [0].fd;
    *waiting_pipe_fd_ = fds [1].fd;

    delete [] fds;

    return pgm_receiver_fd_count;
}

//   Get fds and store them into user allocated memory. 
//   sender_fd is from pgm_transport->send_sock.
//   receive_fd_ is from  transport->recv_sock.
int zmq::pgm_socket_t::get_sender_fds (int *send_fd_, int *receive_fd_)
{

    //  Preallocate pollfds array.
    int fds_array_size = pgm_sender_fd_count;
    pollfd *fds = new pollfd [fds_array_size];
    memset (fds, '\0', fds_array_size * sizeof (fds));

    //  Retrieve pollfds from pgm_transport
    int rc = pgm_transport_poll_info (g_transport, fds, &fds_array_size, 
        POLLOUT | POLLIN);

    //  pgm_transport_poll_info has to return one pollfds for POLLOUT and
    //  second for POLLIN.
    //  Note that fds_array_size parameter can be 
    //  changed inside pgm_transport_poll_info call.
    assert (rc == pgm_sender_fd_count);
 
    //  Store pfds into user allocated space.
    *receive_fd_ = fds [0].fd;
    *send_fd_ = fds [1].fd;

    delete [] fds;

    return pgm_sender_fd_count;
}

//  Send one APDU, transmit window owned memory.
size_t zmq::pgm_socket_t::send (unsigned char *data_, size_t data_len_)
{
    iovec iov = {data_,data_len_};

    ssize_t nbytes = pgm_transport_send_packetv (g_transport, &iov, 1, 
        MSG_DONTWAIT | MSG_WAITALL, true);

    assert (nbytes != -EINVAL);

    if (nbytes == -1 && errno != EAGAIN) {
        errno_assert (false);
    }

    //  If nbytes is -1 and errno is EAGAIN means that we can not send data 
    //  now. We have to call write_one_pkt again.
    nbytes = nbytes == -1 ? 0 : nbytes;

    zmq_log (4, "wrote %iB, %s(%i)\n", (int)nbytes, __FILE__, __LINE__);
    
    // We have to write all data as one packet.
    if (nbytes > 0) {
        assert (nbytes == (ssize_t)data_len_);
    }

    return nbytes;
}

//  Return max TSDU size without fragmentation from current PGM transport.
size_t zmq::pgm_socket_t::get_max_tsdu_size (void)
{
    return (size_t)pgm_transport_max_tsdu (g_transport, false);
}

//  Returns how many APDUs are needed to fill reading buffer.
size_t zmq::pgm_socket_t::get_max_apdu_at_once (size_t readbuf_size_)
{
    assert (readbuf_size_ > 0);

    //  Read max TSDU size without fragmentation.
    size_t max_tsdu_size = get_max_tsdu_size ();

    //  Calculate number of APDUs needed to fill the reading buffer.
    size_t apdu_count = (int)readbuf_size_ / max_tsdu_size;

    if ((int) readbuf_size_ % max_tsdu_size)
        apdu_count ++;

    //  Have to have at least one APDU.
    assert (apdu_count);

    return apdu_count;
}

//  Allocate buffer for one packet from the transmit window, The memory buffer 
//  is owned by the transmit window and so must be returned to the window with 
//  content via pgm_transport_send() calls or unused with pgm_packetv_free1(). 
void *zmq::pgm_socket_t::get_buffer (size_t *size_)
{
    //  Store size.
    *size_ = get_max_tsdu_size ();

    //  Allocate one packet.
    return pgm_packetv_alloc (g_transport, false);
}

//  Return an unused packet allocated from the transmit window 
//  via pgm_packetv_alloc(). 
void zmq::pgm_socket_t::free_buffer (void *data_)
{
    pgm_packetv_free1 (g_transport, data_, false);
}

//  pgm_transport_recvmsgv is called to fill the pgm_msgv array up to 
//  pgm_msgv_len. In subsequent calls data from pgm_msgv structure are 
//  returned.
ssize_t zmq::pgm_socket_t::receive (void **raw_data_)
{
 
    //  We just sent all data from pgm_transport_recvmsgv up 
    //  and have to return 0 that another engine in this thread is scheduled.
    if (nbytes_rec == nbytes_processed && nbytes_rec > 0) {

        //  Reset all the counters.
        nbytes_rec = 0;
        nbytes_processed = 0;
        pgm_msgv_processed = 0;

        return 0;
    }

    //  If we have are going first time or if we have processed all pgm_msgv_t
    //  structure previaously read from the pgm socket.
    if (nbytes_rec == nbytes_processed) {

        //  Check program flow.
        assert (pgm_msgv_processed == 0);
        assert (nbytes_processed == 0);
        assert (nbytes_rec == 0);

        //  Receive a vector of Application Protocol Domain Unit's (APDUs) 
        //  from the transport.
        nbytes_rec = pgm_transport_recvmsgv (g_transport, pgm_msgv, 
            pgm_msgv_len, MSG_DONTWAIT);
  
        //  In a case when no ODATA/RDATA fired POLLIN event (SPM...)
        //  pgm_transport_recvmsg returns -1 with errno == EAGAIN.
        if (nbytes_rec == -1 && errno == EAGAIN) {
        
            //  In case if no RDATA/ODATA caused POLLIN 0 is 
            //  returned.
            nbytes_rec = 0;
            return 0;
        }

        //  For data loss nbytes_rec == -1 errno == ECONNRESET.
        if (nbytes_rec == -1 && errno == ECONNRESET) {
            
            //  In case of dala loss -1 is returned.
            zmq_log (1, "Data loss detected, %s(%i)\n", __FILE__, __LINE__);
            nbytes_rec = 0;
            return -1;
        }

        //  Catch the rest of the errors.
        if (nbytes_rec <= 0) {
            zmq_log (1, "received %i B, errno %i, %s(%i)", (int)nbytes_rec, 
                errno, __FILE__, __LINE__);
            errno_assert (nbytes_rec > 0);
        }
   
        zmq_log (4, "received %i bytes\n", (int)nbytes_rec);
    }

    assert (nbytes_rec > 0);

    // Only one APDU per pgm_msgv_t structure is allowed. 
    assert (pgm_msgv [pgm_msgv_processed].msgv_iovlen == 1);

    //  Take pointers from pgm_msgv_t structure.
    *raw_data_ = pgm_msgv[pgm_msgv_processed].msgv_iov->iov_base;
    size_t raw_data_len = pgm_msgv[pgm_msgv_processed].msgv_iov->iov_len;

    //  Check if peer TSI did not change, this is detection of peer restart.
    const pgm_tsi_t *current_tsi = pgm_msgv [pgm_msgv_processed].msgv_tsi;

    //  If empty store new TSI.
    if (tsi_empty (&tsi)) {
        //  Store current peer TSI.
        memcpy (&tsi, current_tsi, sizeof (pgm_tsi_t));
#ifdef PGM_SOCKET_DEBUG
        uint8_t *gsi = (uint8_t*)(&tsi)->gsi.identifier;
#endif

        zmq_log (1, "First peer TSI: %i.%i.%i.%i.%i.%i.%i, %s(%i)\n",
            gsi [0], gsi [1], gsi [2], gsi [3], gsi [4], gsi [5], 
            ntohs (tsi.sport), __FILE__, __LINE__);
    }

    //  Compare stored TSI with actual.
    if (!tsi_equal (&tsi, current_tsi)) {
        //  Peer change detected.
        zmq_log (1, "Peer change detected, %s(%i)\n", __FILE__, __LINE__);
        
        //  Compare with retired TSI, in case of match ignore APDU.
        if (tsi_equal (&retired_tsi, current_tsi)) {
            zmq_log (1, "Retired TSI - ignoring APDU, %s(%i)\n", 
                __FILE__, __LINE__); 
            
            //  Move the the next pgm_msgv_t structure.
            pgm_msgv_processed++;
            nbytes_processed +=raw_data_len;
            
            return 0;

        } else {
            zmq_log (1, "New TSI, %s(%i)\n", __FILE__, __LINE__); 

            //  Store new TSI and move last valid to retired_tsi
            memcpy (&retired_tsi, &tsi, sizeof (pgm_tsi_t));
            memcpy (&tsi, current_tsi, sizeof (pgm_tsi_t));

#ifdef PGM_SOCKET_DEBUG
            uint8_t *gsi = (uint8_t*)(&retired_tsi)->gsi.identifier;
#endif
            zmq_log (1, "retired TSI: %i.%i.%i.%i.%i.%i.%i, %s(%i)\n",
                gsi [0], gsi [1], gsi [2], gsi [3], gsi [4], gsi [5], 
                ntohs (retired_tsi.sport), __FILE__, __LINE__);

#ifdef PGM_SOCKET_DEBUG
            gsi = (uint8_t*)(&tsi)->gsi.identifier;
#endif
            zmq_log (1, "        TSI: %i.%i.%i.%i.%i.%i.%i, %s(%i)\n",
                gsi [0], gsi [1], gsi [2], gsi [3], gsi [4], gsi [5], 
                ntohs (tsi.sport), __FILE__, __LINE__);

            //  Peers change is recognized as a GAP.
            return -1;
        }

    }
        
    //  Move the the next pgm_msgv_t structure.
    pgm_msgv_processed++;
    nbytes_processed +=raw_data_len;

    zmq_log (4, "sendig up %i bytes\n", (int)raw_data_len);

    return raw_data_len;
}

void zmq::pgm_socket_t::process_upstream (void)
{
    zmq_log (1, "On upstream packet, %s(%i)\n", __FILE__, __LINE__);
    //  We acctually do not want to read any data here we are going to 
    //  process NAK.
    pgm_msgv_t dummy_msg;

    ssize_t dummy_bytes = pgm_transport_recvmsgv (g_transport, &dummy_msg,
        1, MSG_DONTWAIT);
    
    //  No data should be returned.
    assert (dummy_bytes == -1 && errno == EAGAIN);
}

bool zmq::pgm_socket_t::tsi_equal (const pgm_tsi_t *tsi_a_, 
    const pgm_tsi_t *tsi_b_)
{
    //  Compare 6B GSI.
    const uint8_t *gsi_a = tsi_a_->gsi.identifier;
    const uint8_t *gsi_b = tsi_b_->gsi.identifier;

    if (gsi_a [0] != gsi_b [0] || gsi_a [1] != gsi_b [1] || 
          gsi_a [2] != gsi_b [2] || gsi_a [3] != gsi_b [3] ||
          gsi_a [4] != gsi_b [4] || gsi_a [5] != gsi_b [5]) {

        return false;
    }

    //  Compare source port.
    if (tsi_a_->sport != tsi_b_->sport) {
        return false;
    }

    return true;
}

bool zmq::pgm_socket_t::tsi_empty (const pgm_tsi_t *tsi_)
{

    uint8_t *gsi = (uint8_t*)tsi_->gsi.identifier;

    //  GSI.
    if (gsi [0] != 0 || gsi [1] != 0 || gsi [2] != 0 || 
         gsi [3] != 0 || gsi [4] != 0 || gsi [5] != 0) {
        return false;
    }

    //  Source port.
    if (tsi_->sport != 0) {
        return false;
    }

    return true;
}

#else

zmq::pgm_socket_t::pgm_socket_t (bool receiver_, const char *interface_,
      size_t readbuf_size_) :
    receiver (receiver_),
    port_number (0),
    readbuf_size (readbuf_size_),
    nbytes_rec (0),
    nbytes_processed (0),
    pgm_msgv_processed (0),
    created_receiver_socket (false)
{
    zmq_log (1, "interface_  %s, %s(%i)\n", interface_, __FILE__, __LINE__);

    //  Check if we are encapsulating into UDP.
    const char* iface = interface_;
    if (strlen (iface) >= 4 && iface [0] == 'u' &&
        iface [1] == 'd' && iface [2] == 'p' &&
        iface [3] == ':') {

        //  Ms-pgm does not support udp encapsulation.
        assert (false);
    }

    //  Parse port number.
    char *port_delim = strchr ((char *)iface, ':');
    assert (port_delim);
    port_number = atoi (port_delim + 1);
	

    //  Store interface string.
    assert (port_delim > iface);
    assert (port_delim - iface < (int) sizeof (network) - 1);
    memset (network, '\0', sizeof (network));
    memset (multicast, '\0', sizeof (multicast));

    char *network_delim = strchr ((char *)iface, ';');
    assert (network_delim);
    memcpy (network, iface, strlen (iface) - strlen (network_delim));

    memcpy (multicast, network_delim + 1, 
        strlen (iface) - strlen(network) - strlen (port_delim) - 1);

    zmq_log (1, "parsed: network  %s, port %i, %s(%i)\n",
        network, port_number, __FILE__, __LINE__);

    //  Open PGM transport.
    open_transport ();
}

void zmq::pgm_socket_t::open_transport ()
{
    SOCKADDR_IN salocal;
    SOCKADDR_IN sasession;

    zmq_log (1, "Opening PGM: network  %s, port %i, %s(%i)\n",
        network, port_number, __FILE__, __LINE__);

    //  Intialise Windows sockets. Note that WSAStartup can be called multiple
    //  times given that WSACleanup will be called for each WSAStartup.
    WORD version_requested = MAKEWORD (2, 2);
    WSADATA wsa_data;
    int rc = WSAStartup (version_requested, &wsa_data);
    assert (rc == 0);
    assert (LOBYTE (wsa_data.wVersion) == 2 || 
        IBYTE (wsa_data.wVersion) == 2);

    //  Verify version of winsock in use.
    if (LOBYTE(wsa_data.wVersion) != 2 ||
          IBYTE(wsa_data.wVersion) != 2) {
        printf ("Winsock %d.%d is required, this machine provides version %d.%d.\n",
            LOBYTE(wsa_data.wVersion), HIBYTE(wsa_data.wVersion));
        WSACleanup();
        assert (false);
    }

    //  Check for PGM support.
    //  1: query winsock for required buffer size.
    int protocol_list[] = { IPPROTO_RM, 0 };
    WSAPROTOCOL_INFOW* lpProtocolBuf = NULL;
    DWORD dwBufLen = 0;
    int err;
    int protocols = WSCEnumProtocols (protocol_list, lpProtocolBuf, 
        &dwBufLen, &err);
    if (SOCKET_ERROR != protocols) {
        WSACleanup();
        assert (false);
    } else if (WSAENOBUFS != err) {
        WSACleanup();
        assert (false);
    }

    //  2: query with allocated buffer.
    lpProtocolBuf = (WSAPROTOCOL_INFOW*)malloc (dwBufLen);
    if (NULL == lpProtocolBuf) {
        WSACleanup();
        assert (false);
    }
    protocols = WSCEnumProtocols (protocol_list, lpProtocolBuf, 
        &dwBufLen, &err);
    if (SOCKET_ERROR == protocols) {
        free (lpProtocolBuf);
        WSACleanup();
        assert (false);
    }

    bool found = FALSE;
    for (int i = 0; i < protocols; i++) {
        if (AF_INET == lpProtocolBuf[i].iAddressFamily &&
              IPPROTO_RM == lpProtocolBuf[i].iProtocol &&
              SOCK_RDM == lpProtocolBuf[i].iSocketType) {
            found = TRUE;
            break;
        }
    }

    if (!found) {
        puts ("PGM support is not installed on this machine.");
        free (lpProtocolBuf);
        WSACleanup();
        assert (false);
    }

    free (lpProtocolBuf);

    //  Zero counter used in msgrecv.
    nbytes_rec = 0;
    nbytes_processed = 0;
    pgm_msgv_processed = 0;

    //  Receiver transport.
    if (receiver) {

        receiver_listener_socket = socket (AF_INET, SOCK_RDM, IPPROTO_RM);
        wsa_assert (receiver_listener_socket != INVALID_SOCKET);

        // The bind port (port_number) specified should match that
        // which the sender specified in the connect call.
        salocal.sin_family = AF_INET;
        salocal.sin_port   = htons (port_number);
        salocal.sin_addr.s_addr = inet_addr (multicast);

        rc = bind (receiver_listener_socket, (SOCKADDR *) &salocal, 
            sizeof(salocal));
        wsa_assert (rc != SOCKET_ERROR);

        rc = listen (receiver_listener_socket, 10);
        wsa_assert (rc != SOCKET_ERROR);

    //  Sender transport.
    } else {

        sender_socket = socket (AF_INET, SOCK_RDM, IPPROTO_RM);
        wsa_assert (sender_socket != INVALID_SOCKET);
        salocal.sin_family = AF_INET;
        salocal.sin_port   = htons (0);        // Port is ignored here
        salocal.sin_addr.s_addr = htonl (INADDR_ANY);

        int rc = bind (sender_socket, (SOCKADDR *)&salocal, sizeof(salocal));
        wsa_assert (rc != SOCKET_ERROR);
        int to_preallocate = 0;

        // Set all relevant sender socket options here.
        ULONG val = 1;
        setsockopt (sender_socket, IPPROTO_RM, RM_HIGH_SPEED_INTRANET_OPT, 
            (char*)&val, sizeof(val));

        // Set the outgoing interface.
        ULONG send_iface;
        send_iface = inet_addr (network);
        rc = setsockopt (sender_socket, IPPROTO_RM, RM_SET_SEND_IF,
            (char *)&send_iface, sizeof(send_iface));
        wsa_assert (rc != SOCKET_ERROR);

        //  Set window size.
        assert (pgm_max_rte >= 1000);
        assert (pgm_secs != 0);
        RM_SEND_WINDOW send_window;
        send_window.RateKbitsPerSec = pgm_max_rte / 1000 * 8;
        send_window.WindowSizeInMSecs = pgm_secs * 1000;

        //  Parameter WindowSizeInBytes is calculated automaticaly as:
        send_window.WindowSizeInBytes =
            send_window.RateKbitsPerSec * send_window.WindowSizeInMSecs / 8;

        rc = setsockopt (sender_socket, IPPROTO_RM, RM_RATE_WINDOW_SIZE,
            (char *) &send_window, sizeof (send_window));
        wsa_assert (rc != SOCKET_ERROR);

        //  Set multicast address and port number.
        sasession.sin_family = AF_INET;
        sasession.sin_port = htons (port_number);
        sasession.sin_addr.s_addr = inet_addr (multicast);

        rc = connect (sender_socket, (SOCKADDR *)&sasession, 
            sizeof(sasession));
        wsa_assert (rc != SOCKET_ERROR);
    }
}

zmq::pgm_socket_t::~pgm_socket_t ()
{
    close_transport ();
    WSACleanup ();
}

void zmq::pgm_socket_t::close_transport (void)
{
    int rc = shutdown (sender_socket, SD_BOTH);
       wsa_assert (rc);

    if (sender_socket != INVALID_SOCKET)
        closesocket (sender_socket);
    if (receiver_socket != INVALID_SOCKET)
        closesocket (receiver_socket);
}

//   Get receiver fd.
void zmq::pgm_socket_t::get_receiver_fds (int *recv_fd_)
{
    *recv_fd_ = receiver_socket;
}

void zmq::pgm_socket_t::get_receiver_listener_fd (int *list_fd_)
{
    *list_fd_ = receiver_listener_socket;
}

void zmq::pgm_socket_t::set_receiver_fd (int recv_fd_)
{
    receiver_socket = recv_fd_;
}

//   Get fd and store it into user allocated memory.
void zmq::pgm_socket_t::get_sender_fds (int *send_fd_)
{
    *send_fd_ = sender_socket;
}

//  Send one APDU, transmit window owned memory.
size_t zmq::pgm_socket_t::send_data (unsigned char *data_, size_t data_len_)
{
    int nbytes = send (sender_socket, (const char*) data_, data_len_, 0);
    wsa_assert (nbytes != SOCKET_ERROR);

    // We have to write all data as one send.
    if (nbytes > 0) {
        assert (nbytes == (int) data_len_);
    }
    zmq_log (4, "wrote %iB, %s(%i)\n", (int)nbytes, __FILE__, __LINE__);

    return nbytes;
}

//  Return max TSDU size without fragmentation from current PGM transport.
size_t zmq::pgm_socket_t::get_max_tsdu_size (void)
{
    return pgm_max_tpdu;
}

//  Returns how many APDUs are needed to fill reading buffer.
size_t zmq::pgm_socket_t::get_max_apdu_at_once (size_t readbuf_size_)
{
    assert (readbuf_size_ > 0);

    //  Read max TSDU size without fragmentation.
    size_t max_tsdu_size = get_max_tsdu_size ();

    //  Calculate number of APDUs needed to fill the reading buffer.
    size_t apdu_count = (int)readbuf_size_ / max_tsdu_size;

    if ((int) readbuf_size_ % max_tsdu_size)
        apdu_count ++;

    //  Have to have at least one APDU.
    assert (apdu_count);

    return apdu_count;
}

int zmq::pgm_socket_t::receive (void **raw_data_)
{

    //  We just sent all data from pgm_transport_recvmsgv up
    //  and have to return 0 that another engine in this thread is scheduled.
    if (nbytes_rec == nbytes_processed && nbytes_rec > 0) {

        //  Reset all the counters.
        nbytes_rec = 0;
        nbytes_processed = 0;
        pgm_msgv_processed = 0;

        return 0;
    }
    //  If we have are going first time or if we have processed all pgm_msgv_t
    //  structure previaously read from the pgm socket.
    if (nbytes_rec == nbytes_processed) {

        //  Check program flow.
        assert (pgm_msgv_processed == 0);

        //  Receive a vector of Application Protocol Domain Unit's (APDUs)
        //  from the transport.
        nbytes_rec = recv (receiver_socket, pgm_msgv, pgm_max_tpdu, 0);

        if (nbytes_rec == 0) {

            //  Connection terminated.
            zmq_log (1, "Connection terminated\n");
            nbytes_rec = 0;
            return -1;
        }

        if (nbytes_rec == SOCKET_ERROR) {
            nbytes_rec = 0;
            zmq_log (1, "Data Loss\n");
            return -1;
        }

        zmq_log (4, "received %i bytes\n", (int)nbytes_rec);

    }
    assert (nbytes_rec > 0);

    *raw_data_ = pgm_msgv;
    assert (*raw_data_ != NULL);
    int raw_data_len = nbytes_rec;
    nbytes_processed = nbytes_rec;

    zmq_log (4, "sendig up %i bytes\n", (int)raw_data_len);

    return raw_data_len;
}

#endif
#endif
