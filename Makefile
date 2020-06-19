CC=gcc

LibrePIA:
	$(CC) -lz -o LibrePIA LibrePIA.c
	$(CC) -lz -D DEBUG -o LibrePIA-debug LibrePIA.c

.PHONY: clean
clean:
	rm -f LibrePIA
	rm -f LibrePIA-debug
