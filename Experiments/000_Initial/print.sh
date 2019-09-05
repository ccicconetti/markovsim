#!/bin/bash

for i in $@ ; do
  echo -n "$i: "
  cat $i | tr ' ' '\n' | sed -e "/^$/d" | sed -e "/^-/d" | percentile.py --mean
done

for i in $@ ; do
  echo -n "$i: "
  cat $i | tr ' ' '\n' | sed -e "/^$/d" | sed -e "/^-/d" | percentile.py --quantiles 0.95
done

for i in $@ ; do
  echo -n "$i: "
  unstable=$( cat $i | tr ' ' '\n' | sed -e "/^$/d" | grep "^-" | wc -l  )
  total=$( cat $i | tr ' ' '\n' | sed -e "/^$/d" | wc -l  )
  ratio=$( echo "scale=2;$unstable/$total" | bc )
  echo $ratio
done
