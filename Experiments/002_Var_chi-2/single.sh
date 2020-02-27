#!/bin/bash

mkdir single 2> /dev/null

for i in out* ; do
  cat $i | tr ' ' '\n' | sed -e "/^$/d" | sed -e "/^-/d"  > single/$i
done
