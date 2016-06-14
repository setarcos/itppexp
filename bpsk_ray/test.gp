set grid
set logscale y
set xlabel "Eb/N0[db]"
set ylabel "BER"
set xrange [-5:21]
#set format y "10^{%L}"
#set terminal postscript eps enhanced
set title "BER for BPSK over rayleigh fading"
plot "test.dat" using 1:2 w lp pt 5 title "Simulated",\
     "test.dat" using 1:3 w lp pt 7 title "Theoretical"
pause mouse any "Hit any key to exit\r\n"
