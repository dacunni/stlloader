
all: stldump stlcopy

stldump: stldump.cpp stlloader.h
	$(CXX) stldump.cpp -o stldump -std=c++11 -g

stlcopy: stlcopy.cpp stlloader.h
	$(CXX) stlcopy.cpp -o stlcopy -std=c++11 -g

clean:
	rm -f stldump stlcopy

