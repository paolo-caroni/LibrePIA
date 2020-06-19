CC=gcc

LibrePIA:
	$(CC) -lz -o modify src/LibrePIA.c src/modify.c
	$(CC) -lz -D DEBUG -o modify-debug src/LibrePIA.c src/modify.c
	$(CC) -lz -o decompress src/LibrePIA.c src/unzip.c
	$(CC) -lz -D DEBUG -o decompress-debug src/LibrePIA.c src/unzip.c
	$(CC) -lz -o ctb_new src/LibrePIA.c src/new_ctb.c
	$(CC) -lz -D DEBUG -o ctb_new-debug src/LibrePIA.c src/new_ctb.c
	$(CC) -lz -o stb_new src/LibrePIA.c src/new_stb.c
	$(CC) -lz -D DEBUG -o stb_new-debug src/LibrePIA.c src/new_stb.c
	$(CC) -lz -o stb_add_style src/LibrePIA.c src/add_style.c
	$(CC) -lz -D DEBUG -o stb_add_style-debug src/LibrePIA.c src/add_style.c
	$(CC) -lz -o stb_remove_style src/LibrePIA.c src/remove_style.c
	$(CC) -lz -D DEBUG -o stb_remove_style-debug src/LibrePIA.c src/remove_style.c

.PHONY: clean
clean:
	rm -f modify
	rm -f modify-debug
	rm -f decompress
	rm -f decompress-debug
	rm -f ctb_new
	rm -f ctb_new-debug
	rm -f stb_new
	rm -f stb_new-debug
	rm -f stb_add_style
	rm -f stb_add_style-debug
	rm -f stb_remove_style
	rm -f stb_remove_style-debug
