#!/bin/bash

runs=1000
clients="6 7 8 9 10"
chi_values="0.001 0.002 0.005 0.01 0.02 0.05 0.1 0.2 0.3 0.4 0.5"

mkdir raw 2> /dev/null

for c in $clients ; do
  for chi in $chi_values ; do
    echo "$c clients, chi $chi"

    python ../../Serverless/serverless.py \
      --clients $c \
      --servers 6 \
      --chi $chi \
      --mu_min 8 --mu_max 16 \
      --load_min 2 --load_max 2 \
      --threads 1 \
      --runs $runs \
      --output raw/out.chi=$chi.c=$c.dat
  done
done
