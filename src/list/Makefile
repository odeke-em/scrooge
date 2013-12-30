CC := gcc
Scrooge:    LRU.o Scrooge.h Scrooge.c
	$(CC) list.o LRU.o Scrooge.c -o Scrooge

LRU.o:	Makefile list.o list.h LRU.c
	$(CC) -c LRU.c -o LRU.o

list.o:	  Makefile list.h list.c Makefile
	$(CC) -c list.c -o list.o

clean:	  
	rm *.o Scrooge
