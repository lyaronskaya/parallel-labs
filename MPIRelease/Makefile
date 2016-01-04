CC := /opt/openmpi/bin/mpic++
CRUN := /opt/openmpi/bin/mpirun
#CC := g++-4.4
#CC := g++

CC_FLAGS := -std=c++0x 
LD_FLAGS := $(CC_FLAGS) 
RUN_FLAGS := -np 3

BUILD_DIR := build
RELEASE_DIR := $(BUILD_DIR)/release

RELEASE_TARGET := bin/release

SOURCES := $(wildcard 5sem_2task/*.cpp)

RELEASE_OBJECTS := $(patsubst 5sem_2task/%.cpp,$(RELEASE_DIR)/%.o,$(SOURCES))

THREADS := 1 2 3 4 8 16 64 200
TEST_DIR := out
TESTS := $(patsubst %,$(TEST_DIR)/%.out,$(THREADS))

all: release

release: $(RELEASE_TARGET)

run: run-release

run-release: $(RELEASE_TARGET)
	$(RELEASE_TARGET)

timing: $(RELEASE_TARGET)
	$(SHELL) timing.sh $(THREADS)

$(RELEASE_TARGET): $(RELEASE_OBJECTS)
	$(CC) $^ $(LD_FLAGS) -o $

$(RELEASE_DIR)/%.o: MPIRelease/main.cpp
	$(CRUN) $(RUN_FLAGS) $< $(CC_FLAGS) -o $@

clean:
	rm -f $(wildcard $(RELEASE_TARGET))

.PHONY: all release run clean run-release timing

