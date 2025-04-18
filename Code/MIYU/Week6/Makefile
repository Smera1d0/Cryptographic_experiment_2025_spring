CC = g++ -std=c++11

all: check integral

integral: integral.o aes.o 
	$(CC) integral.o aes.o -o integral

check: check.o aes.o 
	$(CC) check.o aes.o -o check

check.o: check.cpp aes.h
	$(CC) -c check.cpp -o check.o

integral.o: integral.cpp aes.h
	$(CC) -c integral.cpp -o integral.o

aes.o: aes.cpp aes.h
	$(CC) -c aes.cpp -o aes.o

.PHONY: clean

clean:
	rm -rf *.o check integral


	


