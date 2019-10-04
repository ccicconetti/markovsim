#!/bin/bash

for i in $@ ; do
  echo -n "$i: "
  percentile.py --mean < $i
done

for i in $@ ; do
  echo -n "$i: "
  percentile.py --quantile 0.95 < $i
done

for i in $@ ; do
  echo -n "$i: "
  unstable=$( cat $i | tr ' ' '\n' | sed -e "/^$/d" | grep "^-" | wc -l  )
  total=$( cat $i | tr ' ' '\n' | sed -e "/^$/d" | wc -l  )
  ratio=$( echo "scale=2;$unstable/$total" | bc )
  echo $ratio
done
