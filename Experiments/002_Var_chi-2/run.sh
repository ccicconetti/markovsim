#!/bin/bash

runs=100
clients="5 6 7 8 9 10 11"
chi_values="0.001 0.002 0.005 0.01 0.02 0.05 0.1 0.2"

for c in $clients ; do
  for chi in $chi_values ; do
    echo "$c clients, chi $chi"

    python ../../Serverless/serverless.py \
      --clients $c \
      --servers 6 \
      --chi $chi \
      --mu_min 8 --mu_max 12 \
      --load_min 1 --load_max 3 \
      --threads 1 \
      --runs $runs \
      --output out.chi=$chi.c=$c.dat
  done
done
