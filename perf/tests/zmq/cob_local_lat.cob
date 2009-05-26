IDENTIFICATION DIVISION.
PROGRAM-ID. COB_LOCAL_LAT WITH IDENT "1.0".
*+
* Created 2009-04-23
*     Copyright (c) 2007-2009 FastMQ Inc.
*
*    This file is part of 0MQ.
*
*    0MQ is free software; you can redistribute it and/or modify it under
*    the terms of the Lesser GNU General Public License as published by
*    the Free Software Foundation; either version 3 of the License, or
*    (at your option) any later version.
*
*    0MQ is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    Lesser GNU General Public License for more details.
*
*    You should have received a copy of the Lesser GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*-

ENVIRONMENT DIVISION.
DATA DIVISION.
WORKING-STORAGE SECTION.

copy "libvmszmq:zmq.lib".

01 zmqUsage	    pic x(80) value "Host MessLength #messages".
01 zmqArgs	    pic x(80) value spaces.
01 zmqHost	    pic x(60) value spaces.
01 zmqMessageBuffI  pic x(8192) value "I".
01 zmqMessageBuffO  pic 9(9)  comp.
01 zmqMessageLenA   pic 9(9).
01 zmqMessageLen    pic 9(9)  comp.
01 zmqNumMessagesA  pic 9(9).
01 zmqNumMessages   pic 9(9)  comp.
01 zmqCounter	    pic 9(9)  comp.
01 zmqObj	    pic 9(9)  comp.
01 zmqEid	    pic 9(9)  comp.
01 zmqQid	    pic 9(9)  comp.
01 zmqStartT	    pic 9(9)  comp.
01 zmqEndT	    pic 9(9)  comp.
01 zmqElapsed	    pic 9(9)  comp.
01 zmqLatency	    pic 9(7)v99  comp.
01 zmqLatencyA	    pic zzzzzz9.99.
01 zmqTime.
   05 zmqHH	    pic 99.
   05 zmqMM	    pic 99.
   05 zmqSS	    pic 99.
   05 zmqMS	    pic 99.
01 ret_l_status	    pic 9(9) comp value 1.

PROCEDURE DIVISION GIVING ret_l_status.
01.
    call "LIB$GET_FOREIGN" using by descriptor zmqArgs
				 by descriptor zmqUsage.

    UNSTRING zmqArgs DELIMITED BY ALL spaces
    	INTO zmqHost zmqMessageLenA zmqNumMessagesA
    END-UNSTRING.

    move zmqMessageLenA  to zmqMessageLen.
    move zmqNumMessagesA to zmqNumMessages.

    call "ZMQ_CREATE" using by descriptor zmqHost, 
                            by reference  zmqObj
		      giving ret_l_status.
    if not ret_l_status success
	exit program.
    call "ZMQ_CREATE_EXCHANGE" using by value      zmqObj
				     by descriptor "EL"
				     by value      zmq_scope_local
				     by value	   0
				     by value	   zmq_style_data_distribution
				     by reference  zmqEid
				giving ret_l_status.
    if not ret_l_status success
	exit program.

    call "ZMQ_CREATE_QUEUE" using by value      zmqObj
				  by descriptor "QL"
				  by value	zmq_scope_local
				  by value	0
				  by value	zmq_no_limit
				  by value	zmq_no_limit
				  by value	zmq_no_swap
				  by reference	zmqQid
			    giving ret_l_status.
    if not ret_l_status success
	exit program.

    call "ZMQ_BIND" using by value      zmqObj
			  by descriptor "EL"
			  by descriptor "QG"
			  by value	0
			  by value	0
		    giving ret_l_status.
    if not ret_l_status success
	exit program.

    call "ZMQ_BIND" using by value      zmqObj
			  by descriptor "EG"
			  by descriptor "QL"
			  by value	0
			  by value	0
		    giving ret_l_status.
    if not ret_l_status success
	exit program.

    accept zmqTime from time.
    compute zmqStartT = ((zmqHH * 3600) + (zmqMM * 60) + zmqSS) * 1000.
    add zmqMS to zmqStartT.
    perform varying zmqCounter from 1 by 1 
	until zmqCounter greater zmqNumMessages
	call "ZMQ_SEND"	using by value zmqObj
			      by value zmqEid
			      by reference zmqMessageBuffI
			      by value     zmqMessageLen
			      by value     1
			giving ret_l_status
	end-call
	if not ret_l_status success
	    exit program
	end-if
	call "ZMQ_RECEIVE"  using by value zmqObj
				  by reference zmqMessageBuffO
				  by reference zmqMessageLen
				  by value     0
				  by value     1
			    giving ret_l_status
	end-call
	if not ret_l_status success
	    exit program
	end-if
	call "ZMQ_FREE" using by value zmqMessageBuffO
			giving ret_l_status
	end-call
	if not ret_l_status success
	    exit program
	end-if
    end-perform.
    accept zmqTime from time.
    compute zmqEndT = ((zmqHH * 3600) + (zmqMM * 60) + zmqSS) * 1000.
    add zmqMS to zmqEndT.
    compute zmqElapsed = zmqEndT - zmqStartT.
*
* Turn into microseconds
*
    compute zmqLatency = zmqElapsed / (zmqNumMessages * 2) * 1000.
    move zmqLatency to zmqLatencyA.
    display zmqNumMessages with conversion, " messages, with a length of ",
	     zmqMessageLen with conversion, " processed in ",
	     zmqElapsed    with conversion, " milliseconds.".
    display "Latency: ", zmqLatency with conversion, " [us]".
    call "ZMQ_DESTROY" using by value zmqObj giving ret_l_status.
    if not ret_l_status success
        exit program
    end-if.

END PROGRAM COB_LOCAL_LAT.
