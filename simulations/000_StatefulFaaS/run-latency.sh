#!/bin/bash

mkdir results 2> /dev/null

for (( clients = 40 ; clients <= 120 ; clients += 10 )) ; do
  ./sfm-latency \
    --arrival-rate=0.075 \
    --clients=$clients \
    --containers=40 \
    --service-time-full=1 \
    --service-time-less=3 \
    --output results/latency-$clients.dat
done
