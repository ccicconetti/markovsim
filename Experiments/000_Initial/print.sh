#!/bin/bash

for i in *.dat ; do
  echo -n "$i: "
  cat $i | tr ' ' '\n' | sed -e "/^$/d" | sed -e "/^-/d" | percentile.py --mean
done

for i in *.dat ; do
  echo -n "$i: "
  cat $i | tr ' ' '\n' | sed -e "/^$/d" | sed -e "/^-/d" | percentile.py --quantiles 0.9 0.95
done

for i in *.dat ; do
  echo -n "$i: "
  cat $i | tr ' ' '\n' | sed -e "/^$/d" | grep "^-" | wc -l 
done
