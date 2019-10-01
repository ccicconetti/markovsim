# set term x11 persist
set grid
set key bottom right

set yrange [0:1]
set xrange [1:3]
set xlabel "Delay"
set ylabel "CDF"

plot "< tr ' ' '\n' < ../raw/out.p=single.c=10.s=6.dat | sed -e '/^$/d' | sed -e '/^-/d' | percentile.py --cdf" u 1:2 w lp pointinterval 100 lw 1 pt 4 title "Single executor", \
     "< tr ' ' '\n' < ../raw/out.p=dual.c=10.s=6.dat | sed -e '/^$/d' | sed -e '/^-/d' | percentile.py --cdf" u 1:2 w lp pointinterval 100 lw 1 pt 6 title "Dual executor"

