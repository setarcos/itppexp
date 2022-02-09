set grid
set logscale y
set xlabel "Eb/N0[db]"
set ylabel "BER"
set title "BER for Polar code"
set format y "10^{%L}"
plot "test.dat" using 1:2 w lp pt 5 title "SC",\
     "test.dat" using 1:3 w lp pt 7 title "SCL-8"
pause mouse any "Hit any key to exit\r\n"
