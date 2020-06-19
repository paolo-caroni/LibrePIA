CC=gcc

LibrePIA:
	$(CC) -lz -o modify src/LibrePIA.c src/modify.c
	$(CC) -lz -D DEBUG -o modify-debug src/LibrePIA.c src/modify.c
	$(CC) -lz -o decompress src/LibrePIA.c src/unzip.c
	$(CC) -lz -D DEBUG -o decompress-debug src/LibrePIA.c src/unzip.c
	$(CC) -lz -o new_ctb src/LibrePIA.c src/new_ctb.c
	$(CC) -lz -D DEBUG -o new_ctb-debug src/LibrePIA.c src/new_ctb.c
	$(CC) -lz -o new_stb src/LibrePIA.c src/new_stb.c
	$(CC) -lz -D DEBUG -o new_stb-debug src/LibrePIA.c src/new_stb.c

.PHONY: clean
clean:
	rm -f modify
	rm -f modify-debug
	rm -f decompress
	rm -f decompress-debug
	rm -f new_ctb
	rm -f new_ctb-debug
	rm -f new_stb
	rm -f new_stb-debug
