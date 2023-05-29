# Makefile for Exploring Maximum Relative Fair Clique

# object files list
obj = Graph_utils.o Graph_reduction.o Graph_heuristic.o Graph_ub.o Graph_baseline.o

# header files list
headers = Graph.h Utility.h Timer.h LinearHeap.h

# compiler config
CC = g++ -std=c++11 -w
OPT = -o2

all: main
.PHONY: all clean

main: main.cpp $(obj) $(headers)
	$(CC) -o main main.cpp $(obj)

%.o: %.cpp $(headers)
	$(CC) -c $< -o $@

clean:
	rm -f main $(obj)
