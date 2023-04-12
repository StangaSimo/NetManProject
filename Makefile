.PHONY: all clean run
.SUFFIXES: .c 

main: src/main.c 
	$(CC) -std=c11 src/main.c -o nmp -lpcap -lncurses

clean: 
	-@rm nmp
	-@rm bin/main.o bin/roaring.o
