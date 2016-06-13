set grid
set xlabel "Lamda"
set ylabel "Throughput"
set title "Slotted Aloha"
set yrange [0:0.4]
plot "test.dat" using 1:2 w lp pt 5 title "Slotted Aloha"
pause mouse any "Hit any key except space to exit\n\r"
