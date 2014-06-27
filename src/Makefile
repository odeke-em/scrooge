CC := gcc

%.o:	%.c
	$(CC) $< -c -o $@

%:	%.c %.o
	$(CC) $< -o $@

HashMap:	HashMap.c RTrie.o
clean:	
	rm -f *.o
