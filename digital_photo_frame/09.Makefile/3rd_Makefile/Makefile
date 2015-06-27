test:a.o b.o
	gcc -o $@ a.o b.o

a.o: a.c a.h

%.o: %.c
	gcc -o $@ -c $<

