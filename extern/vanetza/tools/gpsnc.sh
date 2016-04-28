#!/bin/sh
GPSD_LOCAL_PORT=8051
NMEA_REMOTE_PORT=8052
NC_REMOTE_PID=0
NC_LOCAL_PID=0
FIFO=/tmp/gpsnc_fifo.$$

exit_handler()
{
    kill $NC_REMOTE_PID 2>/dev/null
    kill $NC_LOCAL_PID 2>/dev/null
    rm -f $FIFO
}

trap exit_handler SIGINT SIGTERM

mkfifo $FIFO
nc -k -l ${NMEA_REMOTE_PORT} > $FIFO &
NC_REMOTE_PID=$!
nc -l ${GPSD_LOCAL_PORT} < $FIFO &
NC_LOCAL_PID=$!

sleep 1 # wait for nc ports becoming available
gpsd -n -N -D4 tcp://localhost:${GPSD_LOCAL_PORT}

