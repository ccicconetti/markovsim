#!/bin/bash

runs=100
chi="0.001 0.002 0.005 0.01 0.02 0.05 0.1 0.2 0.5"

mkdir raw 2> /dev/null

for x in $chi ; do

  python ../../Serverless/serverless.py \
    --clients 10 \
    --servers 6 \
    --chi $x \
    --mu_min 16 --mu_max 16 \
    --load_min 1 --load_max 3 \
    --threads 1 \
    --runs $runs \
    --output raw/out.x=$x.dat

done
