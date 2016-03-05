CC=g++
SOURCES=InternalCommands.cpp BasicTasks.cpp xsh.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=xsh

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE):
	$(CC) -o  $@ $(SOURCES) -lcurses

