#set term x11 persist
set grid
set key top left Left

set xlabel "{/Symbol c}"
set ylabel "Ratio of absorbing states"
set xtics 0.1
set ytics (0.002, 0.005, 0.01, 0.02, 0.05, 0.1, 0.2)
set logscale y
set xrange [0.2:0.5]
set yrange [0.002:0.2]

plot \
 '../data/out-6.dat' u 1:($2/1000.0) w lp pt 4 title "6 clients",\
 '../data/out-7.dat' u 1:($2/1000.0) w lp pt 6 title "7 clients",\
 '../data/out-8.dat' u 1:($2/1000.0) w lp pt 8 title "8 clients",\
 '../data/out-9.dat' u 1:($2/1000.0) w lp pt 10 title "9 clients",\
 '../data/out-10.dat' u 1:($2/1000.0) w lp pt 12 title "10 clients"
