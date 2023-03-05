OBJS= project02.o sha256.o
PROG= project02

%.o: %.c 
	gcc -c -g -o $@ $<  

$(PROG): $(OBJS)
	gcc -g -o $@ $^

clean:
	rm -rf $(OBJS) $(PROG)
