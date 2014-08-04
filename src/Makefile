CC := gcc
LIBS := -pthread

%.o:	%.c
	$(CC) $< -c -o $@ $(LIBS)

%:	%.c %.o
	$(CC) $< -o $@

Scrooge:	LRU.o list.o hashList.o RTrie.o MutexCondPair.o
HashMap.o:	HashMap.c RTrie.o

LRU.o:	list.o list/LRU.c
	$(CC) -c list/LRU.c -o LRU.o

list.o:	list/list.c
	$(CC) -c list/list.c -o list.o

MutexCondPair.o:	MutexCondPair.c

clean:	
	rm -f *.o
