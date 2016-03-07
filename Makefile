CC=g++
SOURCES=InternalCommands.cpp BasicTasks.cpp xsh.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=xsh
OS:=$(shell uname)

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE):
	$(CC) -g -o  $@ $(SOURCES) -lcurses

clean:
ifeq "$(OS)" "Windows"
	if [ -a xsh.exe ] ; \
	then \
		rm xsh.exe; \
	fi
else ifeq "$(OS)" "Linux"
	if [ -a xsh ] ; \
	then \
		rm xsh; \
	fi
endif
