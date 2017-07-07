set multiplot layout 2,1
set grid
plot "test.dat" using 2 with lines
set logscale y
set format y "%.0e"
plot "test.dat" using 1 with lines
pause mouse any "Hit any key to exit.\r\n"
