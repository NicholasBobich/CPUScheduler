CC		= gcc
CFLAGS	= -ggdb -Wall -pedantic -std=c11
#CFLAGS 	= -O -Wall -pedantic -std=c11
OBJS	= queue.o printStats.o Source.o
PROG	= a2

a2:	 $(OBJS)
	 $(CC) $(CFLAGS) $(OBJS) -o $(PROG)

queue.o: queue.h a2.h

printStats.o: a2.h

Source.o: queue.h a2.h

clean:
	rm -f *.o core a2
run:
	./a2 a2in.txt
