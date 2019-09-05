#!/bin/bash

policies="single dual"
runs=100
servers="4 6 8"
tot_mu=96

for s in $servers ; do

  mu=$(( tot_mu / s ))

  for p in $policies ; do
    single_flag=
    if [ $p == "single" ] ; then
      single_flag="--single"
    fi

    for (( c = 2 ; c <= 16 ; c+=2 )) ; do
      echo "policy $p, $c clients, $s servers, mu $mu"

      python ../../Serverless/serverless.py \
        --clients $c \
        --servers $s \
        --mu_min $mu --mu_max $mu \
        --load_min 1 --load_max 3 \
        --threads 1 \
        $single_flag \
        --runs $runs \
        --output out.p=$p.c=$c.s=$s.dat
    done
  done
done
