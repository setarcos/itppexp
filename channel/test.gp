set multiplot layout 2,1
set grid
set xrange [1:128]
plot "test.dat" using 1 with impulses
plot "test.dat" using 2 with lines
replot
unset multiplot
pause mouse any "Hit any key to exit.\r\n"
