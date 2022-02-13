set grid
set term wxt size 600,300
set multiplot layout 1,2
set logscale y
set xlabel "Eb/N0[db]"
set ylabel "BER"
set title "BER for Polar code"
set format y "10^{%L}"
plot "sc.dat" using 1:2 w lp pt 5 title "SC",\
     "scl.dat" using 1:2 w lp pt 7 title "SCL-16",\
     "cascl.dat" using 1:2 w lp pt 9 title "CASCL-16"
set ylabel "FER"
set title "FER for Polar code"
plot "sc.dat" using 1:3 w lp pt 5 title "SC",\
     "scl.dat" using 1:3 w lp pt 7 title "SCL-16",\
     "cascl.dat" using 1:3 w lp pt 9 title "CASCL-16"
pause mouse any "Hit any key to exit\r\n"
