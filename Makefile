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
	$(CC) -lz -o export2csv src/LibrePIA.c src/export_csv.c
	$(CC) -lz -D DEBUG -o export2csv-debug src/LibrePIA.c src/export_csv.c
	$(CC) -lz -o export2tsv src/LibrePIA.c src/export_tsv.c
	$(CC) -lz -D DEBUG -o export2tsv-debug src/LibrePIA.c src/export_tsv.c

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
	rm -f export2csv
	rm -f export2csv-debug
	rm -f export2tsv
	rm -f export2tsv-debug
