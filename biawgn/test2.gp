set grid
set xlabel "SNR[db]"
set ylabel "Pe"
set logscale y
set format y "10^{%L}"
set xrange [-8:5]
plot "test.dat" using 2:1 w lp pt 5 title "R=0.25",\
    "test.dat" using 3:1 w lp pt 4 title "R=0.333",\
    "test.dat" using 4:1 w lp pt 3 title "R=0.5",\
    "test.dat" using 5:1 w lp pt 2 title "R=0.666",\
    "test.dat" using 6:1 w lp pt 1 title "R=0.75"
pause mouse any "Hit any key to exit\r\n"
