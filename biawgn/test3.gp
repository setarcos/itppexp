set grid
set xlabel "EbN0[db]"
set ylabel "Capacity bits"
set xrange [-2:30]
set yrange [0:8]
set title "BI-AWGN Channel"
plot "test.dat" using 1:2 w lp pt 3 title "BIAWGN",\
     "test.dat" using 3:4 w lp pt 2 title "Shannon"
pause mouse any "Hit any key to exit\r\n"
