set multiplot layout 2,1
plot "test.dat" using 1 with lines
plot "test.dat" using 2 with lines
pause mouse any "Hit any key to exit.\r\n"
