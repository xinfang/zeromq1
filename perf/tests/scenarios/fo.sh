#!/bin/sh

if [ $# -lt 1 ]; then
    echo "Usage: fo [local | remote subs_id]"
    exit 1
fi

#if [$1 = "local"]; then
#    echo "local"
#else [$1 = "remote"]; then
#    echo "remote"
#else
#    echo "Usage: fo [local | remote subs_id]"
#fi

GL_IP="127.0.0.1"
GL_PORT=5555

REC_IP="127.0.0.1"
REC_PORT=5672

MSG_SIZE_START=1
MSG_SIZE_STEPS=10

N_SUBS=1
RUNS=5

TEST_TIME=5000

LOCAL_THR_BIN="/home/malosek/fast-ref-for-0.3/perf/tests/zmq/local_fo"
REMOTE_THR_BIN="/home/malosek/fast-ref-for-0.3/perf/tests/zmq/remote_fo"

##############################################################################

SYS_LAT_DEN=100
SYS_SLOPE=0.55
SYS_OFF=110
SYS_BREAK=1024

SYS_SLOPE_BIG=0.89
SYS_OFF_BIG=0

if [ $1 = "local" ]; then
    echo "running local"    
    while [ $RUNS -gt 0 ]; do
        for i in `seq 0 $MSG_SIZE_STEPS`;
        do
            let MSG_SIZE=2**$i

            if [ $MSG_SIZE -lt $SYS_BREAK ]; then
                MSG_COUNT=`echo "($TEST_TIME * 100000 ) / ($SYS_SLOPE * $MSG_SIZE + $SYS_OFF)" | bc`
            else
                MSG_COUNT=`echo "($TEST_TIME * 100000 ) / ($SYS_SLOPE_BIG * $MSG_SIZE + $SYS_OFF_BIG)" | bc`
            fi

            $LOCAL_THR_BIN $GL_IP $GL_PORT $REC_IP $REC_PORT $MSG_SIZE $MSG_COUNT $N_SUBS
        done
        let RUNS=RUNS-1 
    done
else
    echo -n "running remote"
    while [ $RUNS -gt 0 ]; do
        echo -n "."
        for i in `seq 0 $MSG_SIZE_STEPS`;
        do
            let MSG_SIZE=2**$i
            echo -n "!"

            if [ $MSG_SIZE -lt $SYS_BREAK ]; then
                MSG_COUNT=`echo "($TEST_TIME * 100000 ) / ($SYS_SLOPE * $MSG_SIZE + $SYS_OFF)" | bc`
            else
                MSG_COUNT=`echo "($TEST_TIME * 100000 ) / ($SYS_SLOPE_BIG * $MSG_SIZE + $SYS_OFF_BIG)" | bc`
            fi

            $REMOTE_THR_BIN $GL_IP $GL_PORT $MSG_SIZE $MSG_COUNT $2
            sleep 1
        done
        let RUNS=RUNS-1 
    done
    echo
fi
