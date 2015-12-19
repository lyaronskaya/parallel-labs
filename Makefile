CC=clang++
#СС=/usr/local/bin/clang-omp
CC_FLAGS=-c -fopenmp -std=c++0x

LD_FLAGS= $(CC_FLAGS)
SOURCES=$(wildcard 5sem_openmp2/*.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=main

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LD_FLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CC_FLAGS) $< -o $@