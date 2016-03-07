CC=g++
SOURCES=InternalCommands.cpp BasicTasks.cpp xsh.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=xsh

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE):
	$(CC) -g -o  $@ $(SOURCES) -lcurses


cleanLinux:
	rm xsh

cleanCgwyn:
	if [ -a xsh.exe ] ; \
    then \
	rm xsh.exe; \
 	fi;

	