set logscale x
set grid
set xlabel "Distance[m]"
set ylabel "Path loss[db]"
set title "Log-distance path loss model, fc=1.5GHz"
set key center top
set yrange [40:110]
plot "test.dat" using 1:2 w lp pt 5 title "n=2",\
     "test.dat" using 1:3 w lp pt 2 title "n=3",\
     "test.dat" using 1:4 w lp pt 7 title "n=6"
pause -1 "Press Enter"
