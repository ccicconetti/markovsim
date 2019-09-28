#!/bin/bash

runs=100
clients="2 8 14"
servers="4 6 8"
tot_mu=96
chi_values="0.001 0.002 0.005 0.01 0.02 0.05 0.1 0.2"

for s in $servers ; do

  mu=$(( tot_mu / s ))

  for c in $clients ; do

    for chi in $chi_values ; do
      echo "$c clients, $s servers, mu $mu, chi $chi"

      python ../../Serverless/serverless.py \
        --clients $c \
        --servers $s \
        --chi $chi \
        --mu_min $mu --mu_max $mu \
        --load_min 1 --load_max 3 \
        --threads 1 \
        --runs $runs \
        --output out.chi=$chi.c=$c.s=$s.dat
    done
  done
done
