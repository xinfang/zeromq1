$! RUN_EXAMPLE_BUTTERFLY.COM
$! 2009-02-25
$!+
$! Set up the necessary commands etc. to run the Butterfly example.
$! See http://www.zeromq.org/tutorials:butterfly for details.
$!-
$ def = "''f$environment("DEFAULT")'"	! keep user's default
$ set def zmqBase	! get to known place
$ set def [.examples.butterfly]
$ defit =  "''f$environment("DEFAULT")'"! make life easy...
$!
$ do_a			:== "$''defit'do_a"
$ do_b			:== "$''defit'do_b"
$ queue			:== "$''defit'queue"
$ receive_replies	:== "$''defit'receive_replies"
$ send_requests		:== "$''defit'send_requests"
$!
$! Put the user back to the default they came from
$!
$ set def 'def
$ exit
