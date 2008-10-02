#!/usr/bin/perl
#
#    Copyright (c) 2007-2008 FastMQ Inc.
#
#    This file is part of 0MQ.
#
#    0MQ is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 3 of the License, or
#    (at your option) any later version.
#
#    0MQ is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# this script sends two SPM messages 
# first SPM with window -1 0 
# second SPM with window 0 0 (lead trail)
#
# receiver should generate NAKs and die on not receiving NCFs


# set this acording pgm_transport_set_nak_ncf_retries ()
my $nak_ncf_retries = 5;

use strict;
use PGM::Test;

BEGIN { require "test.conf.pl"; }

$| = 1;

my $sim = PGM::Test->new(tag => 'sim', host => $config{sim}{host}, cmd => $config{sim}{cmd});

$sim->connect;

sub close_ssh {
	$sim = undef;
	print "finished.\n";
}

$SIG{'INT'} = sub { print "interrupt caught.\n"; close_ssh(); };

print "sim: ready.\n";


$sim->say ("create fake ao");
$sim->say ("bind ao");

print "sim: publish SPM xw_trail 0 txw_lead -1 at spm_sqn 0.\n";
$sim->say ("net send spm ao 0 0 4294967295");

print "sim: publish SPM xw_trail 0 txw_lead 0 at spm_sqn 1.\n";
$sim->say ("net send spm ao 1 0 0");

for (my $i = 0; $i < $nak_ncf_retries; $i++) {
    $sim->wait_for_nak ();
    print "sim: NAK received\n";
}


$sim->disconnect;
close_ssh;
