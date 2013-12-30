CC := gcc
THREAD_LIB := -pthread

Scrooge:    lru.o src/Scrooge.c src/Scrooge.h list.o hashlist.o mutexcondpair.o dtypes.o
	    $(CC) $(THREAD_LIB) exec/list.o exec/lru.o src/Scrooge.c exec/mutexcondpair.o exec/hashlist.o exec/dtypes.o -o Scrooge

mutexcondpair.o:    src/MutexCondPair.h src/MutexCondPair.c
	    $(CC) $(THREAD_LIB) -c src/MutexCondPair.c -o exec/mutexcondpair.o

lru.o:	    list.o src/list/LRU.c src/list/LRU.h
	    $(CC) -c src/list/LRU.c -o exec/lru.o

list.o:	    src/list/list.*
	    $(CC) -c src/list/list.c -o exec/list.o

hashlist.o:   src/hashList.c src/hashList.h
	    $(CC) -c src/hashList.c -o exec/hashlist.o

dtypes.o:     src/DTypes.c src/DTypes.h mutexcondpair.o
	    $(CC) -c src/DTypes.c -o exec/dtypes.o

clean:	  
	    rm -f exec/*.o Scrooge
