CC := gcc
THREAD_LIB := -pthread

Scrooge:    hlru.o src/Scrooge.c src/Scrooge.h hashlist.o mutexcondpair.o
	    $(CC) $(THREAD_LIB) exec/hlru.o src/Scrooge.c exec/mutexcondpair.o exec/hashlist.o -o Scrooge
mutexcondpair.o:    src/MutexCondPair.h src/MutexCondPair.c
	    $(CC) $(THREAD_LIB) -c src/MutexCondPair.c -o exec/mutexcondpair.o

hlru.o:	    hashlist.o src/HLRU.c src/HLRU.h
	    $(CC) -c src/HLRU.c -o exec/hlru.o

hashlist.o:   src/hashList.c src/hashList.h
	    $(CC) -c src/hashList.c -o exec/hashlist.o

clean:	  
	    rm -f exec/*.o Scrooge
