CC=g++
SOURCES=InternalCommands.cpp BasicTasks.cpp xsh.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=xsh
OS:=$(shell uname)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE):
	$(CC) -g -o  $@ $(SOURCES) -lcurses

clean:
	rm -f xsh 
	rm -f xsh.exe
#ifeq "$(OS)" "Windows"
#	if [ -a xsh.exe ] ; \
#	then \
#		rm xsh.exe; \
#	fi
#else ifeq "$(OS)" "Linux"
#	rm -f xsh 
#endif
