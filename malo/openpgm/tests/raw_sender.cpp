#include <cstdio>
#include <cstdlib>
#include <glib.h>
#include <assert.h>
#include <unistd.h>

#include <pgm/pgm.h>

int main (int argc, char **argv)
{

    if (argc < 5) {
        printf ("usage: sender network port message_size nloops\n");
        return 1;
    }

    const char *network_ = argv [1];
    int port_ = atoi (argv [2]);

    int nloops = atoi (argv [4]);
    int message_size = atoi (argv [3]);

    printf ("network \"%s\", port %i, message_size %i, nloops %i\n", 
        network_, port_, message_size, nloops);

    pgm_init ();

    pgm_gsi_t gsi;

    int rc = pgm_create_md5_gsi (&gsi);
	assert (rc == 0);

    struct pgm_sock_mreq recv_smr, send_smr;
    int smr_len = 1;
	rc = pgm_if_parse_transport (network_, AF_INET, &recv_smr, &send_smr, &smr_len);
    assert (rc == 0);
	assert (smr_len == 1);

    pgm_transport_t *g_transport;

    rc = pgm_transport_create (&g_transport, &gsi, port_, &recv_smr, 1, &send_smr);
    assert (rc == 0);

    int g_max_tpdu = 1500;
    int g_sqns = 10;
    int g_max_rte = 400*1000;

    pgm_transport_set_max_tpdu (g_transport, g_max_tpdu);
	pgm_transport_set_txw_sqns (g_transport, g_sqns);
    pgm_transport_set_txw_max_rte (g_transport, g_max_rte);
	pgm_transport_set_rxw_sqns (g_transport, g_sqns);
    pgm_transport_set_hops (g_transport, 16);
	pgm_transport_set_ambient_spm (g_transport, 8192*1000);
    guint spm_heartbeat[] = { 1*1000, 1*1000, 2*1000, 4*1000, 8*1000, 16*1000, 
        32*1000, 64*1000, 128*1000, 256*1000, 512*1000, 1024*1000, 2048*1000, 4096*1000, 8192*1000 };
	pgm_transport_set_heartbeat_spm (g_transport, spm_heartbeat, G_N_ELEMENTS(spm_heartbeat));
    pgm_transport_set_peer_expiry (g_transport, 5*8192*1000);
	pgm_transport_set_spmr_expiry (g_transport, 250*1000);

    rc = pgm_transport_bind (g_transport);
    assert (rc == 0);

    unsigned char *buff = new unsigned char [message_size];
    memset (buff, '-', message_size);
   
    for (int i = 0; i < nloops; i++) {
        ssize_t nbytes = pgm_transport_send (g_transport, (gchar*)buff, message_size, 0);
        printf ("%i. sent %iB\n", i, (int)nbytes);

        sleep (1);
    }

    delete [] buff;

    pgm_transport_destroy (g_transport, TRUE);


    return 0;
}
