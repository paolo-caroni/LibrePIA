CC=gcc

LibrePIA:
	$(CC) -lz -o LibrePIA src/LibrePIA.c
	$(CC) -lz -D DEBUG -o LibrePIA-debug src/LibrePIA.c

.PHONY: clean
clean:
	rm -f LibrePIA
	rm -f LibrePIA-debug
