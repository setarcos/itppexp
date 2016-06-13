set grid
set xlabel "Fading envelop"
set ylabel "PDF"
set title "PDF of rayleigh fading"
plot "test.dat" using 1:2 w lp pt 5
pause mouse any "Hit any key to exit\n\r"
