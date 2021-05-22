set grid
set xlabel "Es/N0[db]"
set ylabel "Capacity bits"
set xrange [-9:10]
set yrange [0:1]
set title "BI-AWGN Channel"
plot "test.dat" using 1:2 w lp pt 3 title "BIAWGN",\
     "test.dat" using 1:3 w lp pt 2 title "BSC",\
     "test.dat" using 1:4 w lp pt 1 title "Shannon"
pause mouse any "Hit any key to exit\r\n"
