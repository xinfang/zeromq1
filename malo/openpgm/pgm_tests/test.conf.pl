# test.conf.pl
use vars qw ( %config );

%config = (
	app => {
		host	=> 'ayaka',
		ip	=> '10.6.28.31',
		cmd	=> '/miru/projects/openpgm/pgm/ref/debug/test/app',
	},
	mon => {
		host	=> 'kiku',
		cmd	=> '/miru/projects/openpgm/pgm/ref/debug/test/monitor',
	},
	sim => {
		host	=> 'localhost',
		cmd	=> '/home/malosek/openPGM/pgm/ref/debug/test/sim -n "eth1;226.0.0.1" -s 7500',
#		cmd	=> '/home/malosek/openPGM/pgm/ref/debug/test/sim -n "eth6;234.5.6.7" -s 123',

	},
);
