CC=gcc

modify:
	$(CC) -lz -o modify src/LibrePIA.c src/modify.c
	$(CC) -lz -D DEBUG -o modify-debug src/LibrePIA.c src/modify.c

.PHONY: clean
clean:
	rm -f modify
	rm -f modify-debug
