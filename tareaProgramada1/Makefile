CXX = g++
CXXFLAGS = -std=c++11 -Wall

all: ip_distributor

ip_distributor: ip_distributor.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f ip_distributor

.PHONY: all clean