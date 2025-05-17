
all: stldump stlcopy

CXXFLAGS=-Wall -std=c++11 -g

stldump: stldump.cpp stlloader.h
	$(CXX) $(CXXFLAGS) stldump.cpp -o stldump

stlcopy: stlcopy.cpp stlloader.h
	$(CXX) $(CXXFLAGS) stlcopy.cpp -o stlcopy

clean:
	rm -f stldump stlcopy

