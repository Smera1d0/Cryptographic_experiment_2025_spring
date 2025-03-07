CC = g++ -std=c++11 -O3 -lm

all: find_4R_dist find_4R_dist_Opt1 find_4R_dist_Opt2 find_4R_dist_Opt3

find_4R_dist_Opt1: find_4R_dist_Opt1.o present.o
	$(CC) find_4R_dist_Opt1.o present.o -o find_4R_dist_Opt1

find_4R_dist_Opt3: find_4R_dist_Opt3.o present.o
	$(CC) find_4R_dist_Opt3.o present.o -o find_4R_dist_Opt3

find_4R_dist_Opt2: find_4R_dist_Opt2.o present.o
	$(CC) find_4R_dist_Opt2.o present.o -o find_4R_dist_Opt2

find_4R_dist: find_4R_dist.o present.o
	$(CC) find_4R_dist.o present.o -o find_4R_dist

find_4R_dist.o: find_4R_dist.cpp present.h
	$(CC) -c find_4R_dist.cpp -o find_4R_dist.o

find_4R_dist_Opt1.o: find_4R_dist_Opt1.cpp present.h
	$(CC) -c find_4R_dist_Opt1.cpp -o find_4R_dist_Opt1.o

find_4R_dist_Opt2.o: find_4R_dist_Opt2.cpp present.h
	$(CC) -c find_4R_dist_Opt2.cpp -o find_4R_dist_Opt2.o

find_4R_dist_Opt3.o: find_4R_dist_Opt3.cpp present.h
	$(CC) -c find_4R_dist_Opt3.cpp -o find_4R_dist_Opt3.o

present.o: present.cpp present.h
	$(CC) -c present.cpp -o present.o

.PHONY: clean

clean:
	rm -rf *.o  


	
