CC=gcc

LibrePIA:
	$(CC) -lz -o modify src/LibrePIA.c src/modify.c
	$(CC) -lz -D DEBUG -o modify-debug src/LibrePIA.c src/modify.c
	$(CC) -lz -o decompress src/LibrePIA.c src/unzip.c
	$(CC) -lz -D DEBUG -o decompress-debug src/LibrePIA.c src/unzip.c

.PHONY: clean
clean:
	rm -f modify
	rm -f modify-debug
	rm -f decompress
	rm -f decompress-debug
