#!/bin/bash

policies="single dual"
servers="4 6 8"
tot_mu=96

quantiles="0.5 0.9 0.95 0.98 mean"

mkdir data 2> /dev/null

for s in $servers ; do
  for p in $policies ; do
    rm -f data/*.p=$p.s=$s.dat
    for (( c = 2 ; c <= 14 ; c+=2 )) ; do
      echo "policy $p, $c clients, $s servers"
      infile=raw/out.p=$p.c=$c.s=$s.dat
      for q in $quantiles ; do
        outfile=data/$q.p=$p.s=$s.dat
        if [ $q == "mean" ] ; then
          value=$(tr ' ' '\n' < $infile | sed -e "/^$/d" | sed -e "/^-/d" | \
            percentile.py --mean | cut -f 1,3 -d ' ')
        else
          value=$(tr ' ' '\n' < $infile | sed -e "/^$/d" | sed -e "/^-/d" | \
            percentile.py --quantiles $q | cut -f 2 -d ' ')
        fi
        echo $c $value >> $outfile
      done
    done
  done
done
