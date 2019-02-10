

stldump: stldump.cpp stlloader.h
	$(CXX) stldump.cpp -o stldump -std=c++11 -g

clean: rm -f stldump



