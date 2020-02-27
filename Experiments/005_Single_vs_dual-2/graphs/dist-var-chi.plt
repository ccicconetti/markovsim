# set term x11 persist
set grid
set key bottom right Left

set yrange [0:1]
set xrange [1.2:2]
set xlabel "Delay"
set ylabel "CDF"
set pointsize 2

plot \
  "< tr ' ' '\n' < ../raw/out.x=0.001.dat | sed -e '/^$/d' | sed -e '/^-/d' | percentile.py --cdf" u 1:2 w lp pointinterval 100 lw 1 pt 4 title "{/Symbol c} = 0.001", \
  "< tr ' ' '\n' < ../raw/out.x=0.01.dat | sed -e '/^$/d' | sed -e '/^-/d' | percentile.py --cdf" u 1:2 w lp pointinterval 100 lw 1 pt 6 title "{/Symbol c} = 0.01", \
  "< tr ' ' '\n' < ../raw/out.x=0.1.dat | sed -e '/^$/d' | sed -e '/^-/d' | percentile.py --cdf" u 1:2 w lp pointinterval 100 lw 1 pt 8 title "{/Symbol c} = 0.1", \
  "< tr ' ' '\n' < ../raw/out.x=0.2.dat | sed -e '/^$/d' | sed -e '/^-/d' | percentile.py --cdf" u 1:2 w lp pointinterval 100 lw 1 pt 10 title "{/Symbol c} = 0.2", \
  "< tr ' ' '\n' < ../raw/out.x=0.5.dat | sed -e '/^$/d' | sed -e '/^-/d' | percentile.py --cdf" u 1:2 w lp pointinterval 100 lw 1 pt 12 title "{/Symbol c} = 0.5"

