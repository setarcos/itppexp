run: $(PROG).cpp
	g++ `itpp-config --cflags` -o run $(PROG).cpp `itpp-config --libs`
	./run
draw:
	gnuplot test.gp
debug:
	g++ `itpp-config --debug --cflags` -o run_debug $(PROG).cpp `itpp-config --debug --libs`
clean:
	rm run run_debug -f
