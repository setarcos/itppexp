PROG=main
show: $(PROG)
	gnuplot test.gp
%.o: %.cpp polar.h
	g++ `itpp-config --cflags` -c $< -o $@
$(PROG): $(PROG).o polar.o
	g++ polar.o $(PROG).o `itpp-config --libs ` -o $(PROG)
clean:
	rm *.o $(PROG) -f
