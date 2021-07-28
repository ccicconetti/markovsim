#!/bin/bash

mkdir results 2> /dev/null

for (( clients = 1 ; clients <= 100 ; clients += 1 )) ; do
  ret=$(./sfm-provisioning \
    --arrival-rate=0.075 \
    --clients=$clients \
    --service-time-full=1 \
    --service-time-less=3 \
    --q-full 20 \
    --q-less 80)
  echo "$clients $ret" >> results/provisioning.dat
done
