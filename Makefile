CC := g++
CC_FLAGS := -std=c++0x
LD_FLAGS := $(CC_FLAGS) -fopenmp -lpthread
WORKERS_CNT := 1 2 3 4 8 16 64 128
TEST_DIR := out
TESTS := $(patsubst %,$(TEST_DIR)/%.out,$(THREADS))

all: release

release:

timing: main

all:
	g++ 5sem_openmp2/main.cpp -o main.out -fopenmp -lpthread -std=c++0x

	