#include <assert.h>
#include <cstdio>
#include <unistd.h>

#include <glib.h>

#include <pgm/transport.h>
#include <pgm/async.h>
#include <pgm/gsi.h>
#include <pgm/if.h>
#include <pgm/msgv.h>
#include <pgm/sockaddr.h>

int main (int argc, char *argv [])
{
	pgm_init ();

    pgm_gsi_t gsi;

	int rc = pgm_create_md5_gsi (&gsi);
	assert (rc == 0);

    char* g_network = "eth3;226.0.0.1";
    struct pgm_sock_mreq recv_smr, send_smr;
	int smr_len = 1;
	rc = pgm_if_parse_transport (g_network, AF_INET, &recv_smr, &send_smr, &smr_len);
	assert (rc == 0);
	assert (smr_len == 1);

//    ((struct sockaddr_in*)&send_smr.smr_multiaddr)->sin_port = g_htons (g_udp_encap_port);
//	((struct sockaddr_in*)&recv_smr.smr_interface)->sin_port = g_htons (g_udp_encap_port);

    pgm_transport_t* g_transport = NULL;

    int g_port = 7500;

    rc = pgm_transport_create (&g_transport, &gsi, g_port, &recv_smr, 1, &send_smr);
    if (rc != 0) {
        perror (NULL);
        assert (rc == 0);
    }

    int g_max_tpdu = 1500;
    int g_sqns = 10;
    int g_max_rte = 400*1000;

	pgm_transport_set_max_tpdu (g_transport, g_max_tpdu);
	pgm_transport_set_txw_sqns (g_transport, g_sqns);
	pgm_transport_set_txw_max_rte (g_transport, g_max_rte);
	pgm_transport_set_rxw_sqns (g_transport, g_sqns);
	pgm_transport_set_hops (g_transport, 16);
	pgm_transport_set_ambient_spm (g_transport, 8192*1000);
	guint spm_heartbeat[] = { 1*1000, 1*1000, 2*1000, 4*1000, 8*1000, 16*1000, 32*1000, 64*1000, 128*1000, 256*1000, 512*1000, 1024*1000, 2048*1000, 4096*1000, 8192*1000 };
	pgm_transport_set_heartbeat_spm (g_transport, spm_heartbeat, G_N_ELEMENTS(spm_heartbeat));
	pgm_transport_set_peer_expiry (g_transport, 5*8192*1000);
	pgm_transport_set_spmr_expiry (g_transport, 250*1000);

	rc = pgm_transport_bind (g_transport);
    assert (rc == 0);

/*	if (e != 0) {
		g_critical ("pgm_transport_bind failed errno %i: \"%s\"", e, strerror(e));
		G_BREAKPOINT();
	}
*/
    char buff [128];

    for (int i = 0; i < 10; i++) {
       snprintf (buff, sizeof (buff), "%i", i); 
       rc = pgm_transport_send (g_transport, buff, strlen(buff) + 1, 0);
       sleep (1);
    }

	pgm_transport_destroy (g_transport, TRUE);

    return 0;
}
