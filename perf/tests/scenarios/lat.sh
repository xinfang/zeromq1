#!/bin/sh

if [ $# -ne 2 ]; then
    echo "Usage: lat.sh [tcp | zmq] [local | remote]"
    exit 1
fi

if [ $2 != "local" -a $2 != "remote" ]; then
    echo "Usage: lat.sh [tcp | zmq ] [local | remote]"
    exit 1
fi

if [ $1 != "zmq" -a $1 != "tcp" ]; then
    echo "Usage: lat.sh [tcp | zmq ] [local | remote]"
    exit 1
fi

##############################################################################

GL_IP="62.176.172.203"
GL_PORT=5555

REC_IP="10.0.0.2"
REC_PORT=5672

MSG_SIZE_START=1
MSG_SIZE_STEPS=10

RUNS=3

TEST_TIME=250

LOCAL_LAT_BIN="./local_lat"
REMOTE_LAT_BIN="./remote_lat"

##############################################################################

SYS_LAT_DEN=100
SYS_SLOPE=0.55
SYS_OFF=110
SYS_BREAK=1024

SYS_SLOPE_BIG=0.89
SYS_OFF_BIG=0

if [ $2 = "local" ]; then
    echo "running local (receiver)"    
    while [ $RUNS -gt 0 ]; do
        for i in `seq 0 $MSG_SIZE_STEPS`;
        do
            let MSG_SIZE=2**$i

            if [ $MSG_SIZE -lt $SYS_BREAK ]; then
                MSG_COUNT=`echo "($TEST_TIME * 100000 ) / ($SYS_SLOPE * $MSG_SIZE + $SYS_OFF)" | bc`
            else
                MSG_COUNT=`echo "($TEST_TIME * 100000 ) / ($SYS_SLOPE_BIG * $MSG_SIZE + $SYS_OFF_BIG)" | bc`
            fi

            if [ $1 = "zmq" ]; then
                $LOCAL_LAT_BIN $GL_IP $GL_PORT $REC_IP $REC_PORT $MSG_SIZE $MSG_COUNT $THREADS
            else
                $LOCAL_LAT_BIN $REC_IP $REC_PORT $MSG_SIZE $MSG_COUNT
                let REC_PORT=REC_PORT+1
            fi
        done
        let RUNS=RUNS-1 
    done
else
    echo "running remote (sender)"
    while [ $RUNS -gt 0 ]; do
        for i in `seq 0 $MSG_SIZE_STEPS`;
        do
            let MSG_SIZE=2**$i

            if [ $MSG_SIZE -lt $SYS_BREAK ]; then
                MSG_COUNT=`echo "($TEST_TIME * 100000 ) / ($SYS_SLOPE * $MSG_SIZE + $SYS_OFF)" | bc`
            else
                MSG_COUNT=`echo "($TEST_TIME * 100000 ) / ($SYS_SLOPE_BIG * $MSG_SIZE + $SYS_OFF_BIG)" | bc`
            fi

            if [ $1 = "zmq" ]; then
                $REMOTE_THR_BIN $GL_IP $GL_PORT $MSG_SIZE $MSG_COUNT $THREADS
            else
                $REMOTE_LAT_BIN $REC_IP $REC_PORT $MSG_SIZE $MSG_COUNT
                let REC_PORT=REC_PORT+1
            fi
            sleep 1
        done
        let RUNS=RUNS-1 
    done
    echo
fi
