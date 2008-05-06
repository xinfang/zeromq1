#!/usr/bin/perl

use strict;
use PGM::Test;
use Time::HiRes qw(usleep);

BEGIN { require "test.conf.pl"; }

$| = 1;

my $sim = PGM::Test->new(tag => 'sim', host => $config{sim}{host}, cmd => $config{sim}{cmd});
#my $app = PGM::Test->new(tag => 'app', host => $config{app}{host}, cmd => $config{app}{cmd});

$sim->connect;
#$app->connect;

sub close_ssh {
	$sim = undef;
	print "finished.\n";
}

$SIG{'INT'} = sub { print "interrupt caught.\n"; close_ssh(); };

#$mon->say ("filter $config{app}{ip}");
print "sim: ready.\n";


$sim->say ("create fake ao");
$sim->say ("bind ao");

#sleep (1);
print "sim: publish SPM xw_trail 0 txw_lead -1 at spm_sqn 0.\n";
$sim->say ("net send spm ao 0 0 4294967295");

sleep (1);
print "sim: publish SPM xw_trail 0 txw_lead -1 at spm_sqn 1.\n";
$sim->say ("net send spm ao 1 0 4294967295");

sleep (2);

$sim->say ("net send odata ao 2 0 0 2abcd");
usleep (1000);
$sim->say ("net send odata ao 3 0 4294967295 3abcd");
usleep (1000);
$sim->say ("net send odata ao 0 0 4294967295 0abcd");
usleep (1000);
$sim->say ("net send odata ao 1 0 4294967295 1abcd");
usleep (1000);

#$sim->say ("net send spm ao 30 20 30");


#print "sim: waiting for valid NAK.\n";
#$nak = $sim->wait_for_nak;
#print "sim: NAK received.\n";

#print "sim: waiting for valid NAK.\n";
#$nak = $sim->wait_for_nak;
#print "sim: NAK received.\n";

#print "sim: sending NCF\n";
#$sim->say ("net send ncf ao $nak->{PGM}->{gsi}.$nak->{PGM}->{sourcePort} 1");

sleep (10);
#print "app: ready.\n";
#
#print "mon: wait for spm ...\n";
#$mon->wait_for_spm;
#print "mon: received spm.\n";
#
#print "test completed successfully.\n";
#
#$mon->disconnect (1);
$sim->disconnect;
close_ssh;

# eof
