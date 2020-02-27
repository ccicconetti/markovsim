# set term x11 persist
set grid
set key top left

set yrange [0:5]
set xrange [2:14]
set xlabel "Number of clients"
set ylabel "95th percentile of average delay"
set pointsize 2

set multiplot layout 1,3 rowsfirst

set title "4 servers"
 plot '../data/0.95.p=single.s=4.dat' u 1:2 w lp title "Single executor" pt 4,\
 '../data/0.95.p=dual.s=4.dat' u 1:2 w lp title "Dual executor" pt 6

set title "6 servers"
 plot '../data/0.95.p=single.s=6.dat' u 1:2 w lp title "Single executor" pt 4,\
 '../data/0.95.p=dual.s=6.dat' u 1:2 w lp title "Dual executor" pt 6

set title "8 servers"
 plot '../data/0.95.p=single.s=8.dat' u 1:2 w lp title "Single executor" pt 4,\
 '../data/0.95.p=dual.s=8.dat' u 1:2 w lp title "Dual executor" pt 6

unset multiplot
