.PHONY: all clean run
.SUFFIXES: .c 

main: src/main.c 
		$(CC) -Wall src/main.c -o nmp -lpcap

clean: 
	-@rm nmp
