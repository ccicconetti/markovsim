#!/bin/bash

clients="6 7 8 9 10"
chi_values="0.001 0.002 0.005 0.01 0.02 0.05 0.1 0.2"
quantiles="0.5 0.9 0.95 0.98 0.99"

mkdir data 2> /dev/null

for q in $quantiles ; do
  echo $q
  for c in $clients ; do
    outfile=data/out-$q-$c.dat
    rm $outfile 2> /dev/null

    for chi in $chi_values ; do

      value=$(cat out.chi=$chi.c=$c.dat | tr ' ' '\n' | \
        sed -e "/^$/d" | sed -e "/^-/d" |\
        percentile.py --quantiles $q | cut -d ' ' -f 2)

      echo $chi $value >> $outfile

    done
  done
done
