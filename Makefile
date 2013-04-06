CC=g++
CFLAGS=-c -Wall -ansi -fexceptions -g
LIBS=-lGmsh
SOURCES=Face.cpp HTypeManager.cpp main.cpp Malha.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=H-TypeRefiner

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LIBS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@ 

clean:
	rm -rf *o H-TypeRefiner