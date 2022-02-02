PROG=main
show: run
	gnuplot test.gp
run: $(PROG).cpp polar.cpp polar.h
	g++ `itpp-config --cflags` -c polar.cpp
	g++ `itpp-config --cflags` -c $(PROG).cpp
	g++ polar.o $(PROG).o `itpp-config --libs ` -o run
	./run
clean:
	rm run run_debug test.dat -f
