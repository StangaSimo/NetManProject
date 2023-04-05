.PHONY: all clean run
.SUFFIXES: .c 

main: src/main.c 
		$(CC) src/roaring.c -o bin/roaring.o -c 
		$(CC) -Wall src/main.c -o bin/main.o -c  
		$(CC) -Wall bin/main.o bin/roaring.o -o nmp -lpcap

clean: 
	-@rm nmp
	-@rm bin/main.o bin/roaring.o
