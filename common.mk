$(PROG):
	g++ `itpp-config --cflags` -o $(PROG) $(PROG).cpp `itpp-config --libs`
	./$(PROG)
debug:
	g++ `itpp-config --debug --cflags` -o $(PROG)_debug $(PROG).cpp `itpp-config --debug --libs`
clean:
	rm $(PROG) -f
