.PHONY: all clean run
.SUFFIXES: .c 

#BIN = bin/
##SRC = src/
#S = server/
#C = client/
#JSONS = -classpath server/lib/gson-2.10.jar 
#
#JAVASERVER = -cp $(S)lib/gson-2.10.jar:$(S)bin/:. $(MAINS)
#JAVACLIENT = -cp $(C)bin/ $(MAINC)
#
#MAINS = ServerMain
#MAINC = ClientMain
#
ARG = -i en0  

SRC = src/
MAIN = main
TARGET = $(SRC)$(MAIN) 

#%: %.java
#
#	@javac $(JSONS) $(JAVACFLAG) $(S)$(BIN) $(S)$(SRC)*.java 
#	@javac $(JAVACFLAG) $(C)$(BIN) $(C)$(SRC)*.java 

all: $(TARGET)

nmp: src/main.c 
		$(CC) -Wall src/main.c -o nmp -lpcap
clean: 
	-@rm nmp
