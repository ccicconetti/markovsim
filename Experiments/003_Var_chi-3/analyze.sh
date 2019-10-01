#!/bin/bash

clients="6 7 8 9 10"
chi_values="0.001 0.002 0.005 0.01 0.02 0.05 0.1 0.2"
quantiles="0.5 0.9 0.95 0.98 0.99 mean"

mkdir data 2> /dev/null

for q in $quantiles ; do
  echo $q
  for c in $clients ; do
    outfile=data/out-$q-$c.dat
    rm $outfile 2> /dev/null

    numlines=$(( $c * 1000 * 95 / 100 ))

    for chi in $chi_values ; do
      infile=raw/out.chi=$chi.c=$c.dat

      if [ $q == "mean" ] ; then
        value=$(cat $infile | tr ' ' '\n' | \
          sed -e "/^$/d" | sed -e "/^-/d" |\
          sort -n | head -n $numlines |\
          percentile.py --mean | cut -d ' ' -f 1,3)
      else
        value=$(cat $infile | tr ' ' '\n' | \
          sed -e "/^$/d" | sed -e "/^-/d" |\
          percentile.py --quantiles $q | cut -d ' ' -f 2)
      fi

      echo $chi $value >> $outfile

    done
  done
done
