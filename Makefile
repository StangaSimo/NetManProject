.PHONY: all clean run
.SUFFIXES: .c 

main: src/main.c 
	$(CC) -g -pedantic src/main.c -o nmp -lrrd -lpcap -lncurses 

clean: 
	-@rm nmp
	-@rm bin/main.o bin/roaring.o
