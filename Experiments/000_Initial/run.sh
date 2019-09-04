#!/bin/bash

single_dual="single dual"
runs=100
scenario="a b"

for x in $scenario ; do
  
  if [ $x == "a" ] ; then
    mu=12
    servers=6
  elif [ $x == "b" ] ; then
    mu=16
    servers=8
  fi

  for s in $single_dual ; do
    single_flag=
    if [ $s == "single" ] ; then
      single_flag="--single"
    fi

    for (( c = 2 ; c <= 16 ; c+=2 )) ; do
      echo "$s, $c clients, servers $servers, mu $mu"

      python ../../Serverless/serverless.py \
        --clients $c \
        --servers $servers \
        --mu_min $mu --mu_max $mu \
        --load_min 1 --load_max 3 \
        --threads 1 \
        $single_flag \
        --runs $runs \
        --output out.$a.s=$s.c=$c.dat
    done
  done
done
