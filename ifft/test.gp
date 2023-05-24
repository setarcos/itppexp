set multiplot layout 3,1
set grid
plot "test.dat" using 3 with lines
plot "test.dat" using 2 with lines
plot "test.dat" using 1 with lines
pause mouse any "Hit any key to exit.\r\n"
