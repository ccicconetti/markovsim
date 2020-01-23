#set term x11 persist
set grid
set key bmargin center horizontal

set xlabel "{/Symbol c}"
set ylabel "Median of average delays"
set xtics (0.001, 0.002, 0.005, 0.01, 0.02, 0.05, 0.1, 0.2, 0.5)
set logscale x
set xrange [0.001:0.5]
set pointsize 2

plot \
 '../data/out-0.5-6.dat' u 1:2 w lp pt 4 title "6 clients",\
 '../data/out-0.5-7.dat' u 1:2 w lp pt 6 title "7 clients",\
 '../data/out-0.5-8.dat' u 1:2 w lp pt 8 title "8 clients",\
 '../data/out-0.5-9.dat' u 1:2 w lp pt 10 title "9 clients",\
 '../data/out-0.5-10.dat' u 1:2 w lp pt 12 title "10 clients"
