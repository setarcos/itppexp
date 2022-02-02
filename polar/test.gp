set grid
set logscale y
set xlabel "Eb/N0[db]"
set ylabel "BER"
set title "BER for Polar code"
set format y "10^{%L}"
plot "test.dat" using 1:2 w lp pt 5 title "Simulated"
pause mouse any "Hit any key to exit\r\n"
