#!/bin/bash

mkdir results 2> /dev/null

qf_values="20 30 40"

for qf in $qf_values ; do
  rm -f results/provisioning-$qf.dat 2> /dev/null
  ql=$((100-qf))
  for (( clients = 5 ; clients <= 100 ; clients += 1 )) ; do
    echo "qf $qf, ql $ql, clients $clients"
    ret=$(./sfm-provisioning \
      --arrival-rate=0.075 \
      --clients=$clients \
      --service-time-full=1 \
      --service-time-less=3 \
      --q-full $qf \
      --q-less $ql)
    echo "$clients $ret" >> results/provisioning-$qf.dat
  done
done
