CC=g++
SOURCES=InternalCommands.cpp BasicTasks.cpp ExternalCommands.cpp xsh.cpp 
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=xsh
EXECUTABLES = xsh xsh.exe
OS:=$(shell uname)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE):
	$(CC) -std=c++11 -g -o  $@ $(SOURCES) -lcurses

clean:
	rm -f $(EXECUTABLES) 
