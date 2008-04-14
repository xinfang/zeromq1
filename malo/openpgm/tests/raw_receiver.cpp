#include <cstdio>
#include <cstdlib>
#include <glib.h>
#include <assert.h>
#include <unistd.h>

#include <pgm/pgm.h>


pgm_transport_t *g_transport;

void init (const char *network_, int port_);
void destroy ();


int main (int argc, char **argv)
{

    if (argc != 3) {
        printf ("usage: raw_receiver network port\n");
        return 1;
    }

    const char *network = argv [1];
    int port = atoi (argv [2]);

    init (network, port); 
    
    unsigned char buff [1024];

    ssize_t nbytes = pgm_transport_recv (g_transport, buff, sizeof (buff), 0 );
    assert (nbytes != -1);

    destroy ();

    return 0;
}

void destroy ()
{
    pgm_transport_destroy (g_transport, TRUE);
}

void init (const char *network_, int port_)
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
    assert (rc == 0);

    int g_max_tpdu = 1500;
    int g_sqns = 10;

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
    pgm_transport_set_nak_bo_ivl (g_transport, 50*1000);
    pgm_transport_set_nak_rpt_ivl (g_transport, 200*1000);
    pgm_transport_set_nak_rdata_ivl (g_transport, 200*1000);
    pgm_transport_set_nak_data_retries (g_transport, 5);
    pgm_transport_set_nak_ncf_retries (g_transport, 2);

    // Do not create transmit window and sdo not send SPM broadcast
    //pgm_transport_set_recv_only (g_transport, FALSE);

    rc = pgm_transport_bind (g_transport);
    assert (rc == 0);

    printf ("TSI: %s\n", pgm_print_tsi (&g_transport->tsi));
}
