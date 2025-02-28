CC = g++ -std=c++17 -O3

all: check distinguisher keyrecovery DTT

keyrecovery: keyrecovery.o present.o
	$(CC) keyrecovery.o present.o -o keyrecovery

distinguisher: distinguisher.o present.o
	$(CC) distinguisher.o present.o -o distinguisher

check: check.o present.o 
	$(CC) check.o present.o -o check

check.o: check.cpp present.h
	$(CC) -c check.cpp -o check.o

present.o: present.cpp present.h
	$(CC) -c present.cpp -o present.o

distinguisher.o: distinguisher.cpp present.h
	$(CC) -c distinguisher.cpp -o distinguisher.o

keyrecovery.o: keyrecovery.cpp present.h
	$(CC) -c keyrecovery.cpp -o keyrecovery.o

DTT:
	$(CC) -o DTT

.PHONY: clean

clean:
	rm -rf *.o check distinguisher 


	
