#!/bin/bash

clients="6 7 8 9 10"
chi_values="0.001 0.002 0.005 0.01 0.02 0.05 0.1 0.2 0.3 0.4 0.5"

mkdir data 2> /dev/null

for c in $clients ; do
  outfile=data/out-$c.dat
  rm $outfile 2> /dev/null

  for chi in $chi_values ; do
    infile=raw/out.chi=$chi.c=$c.dat
    value=$(cat $infile)

    echo $chi $value >> $outfile

  done
done
