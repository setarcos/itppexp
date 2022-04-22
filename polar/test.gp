# sc.dat:   ./main -n 2048 -f 1 -l 0 -o sc.dat
# scl.dat:  ./main -n 2048 -f 1 -l 32 -o scl.dat
# cascl.dat:./main -n 2048 -f 1 -l 32 -c 16 -C CCITT-16 -o cascl.dat
set grid
set term qt size 900,450
set multiplot layout 1,2
set logscale y
set xlabel "Eb/N0[db]"
set ylabel "BER"
set title "BER for Polar code"
set format y "10^{%L}"
plot "sc.dat" using 1:2 w lp pt 5 title "SC",\
     "scl.dat" using 1:2 w lp pt 7 title "SCL-32",\
     "cascl.dat" using 1:2 w lp pt 9 title "CASCL-32"
set ylabel "FER"
set title "FER for Polar code"
plot "sc.dat" using 1:3 w lp pt 5 title "SC",\
     "scl.dat" using 1:3 w lp pt 7 title "SCL-32",\
     "cascl.dat" using 1:3 w lp pt 9 title "CASCL-32"
pause mouse any "Hit any key to exit\r\n"
