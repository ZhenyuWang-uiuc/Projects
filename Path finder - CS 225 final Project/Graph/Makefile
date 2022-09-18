EXENAME = Graph
OBJS = Graph.o main.o

CXX = clang++
CXXFLAGS = $(CS225) -std=c++1y -stdlib=libc++ -c -g -O0 -Wall -Wextra -pedantic
LD = clang++
LDFLAGS = -std=c++1y -stdlib=libc++ -lc++abi -lm

.PHONY: all test clean graph

all : $(EXENAME)

graph: ; $(CLANG_VERSION_MSG)

$(EXENAME): graph $(OBJS)
	$(LD) $(OBJS) $(LDFLAGS) -o $(EXENAME)

Graph.o: main.cpp Graph.cpp Graph.h
	$(CXX) $(CXXFLAGS) main.cpp Graph.cpp

tests.o: myTest/tests.cpp myTest/catch.hpp Graph.h
	$(CXX) $(CXXFLAGS) myTest/tests.cpp

test: graph tests.o Graph.o
	$(LD) tests.o Graph.o $(LDFLAGS) -o test

clean:
	-rm -f *.o $(EXENAME) test
