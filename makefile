CXX = g++
CXXFLAGS = -std=c++2a -Wall -Wextra -O3
LIBS = -lOpenCL

.PHONY: all clean

all: main

main: main.cpp 
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

philox432.o:
	gcc -c clRNG/*.c $^ $(LIBS)

clean:
	rm -rf *.o main
