CC = g++ -std=c++11

all: check aes_encrypt aes_decrypt aes_encrypt_cbc aes_decrypt_cbc

aes_encrypt: aes_encrypt.o aes.o aes.h
	$(CC) aes_encrypt.o aes.o -o aes_encrypt

aes_encrypt_cbc: aes_encrypt_cbc.o aes.o aes.h
	$(CC) aes_encrypt_cbc.o aes.o -o aes_encrypt_cbc

aes_decrypt: aes_decrypt.o aes.o aes.h
	$(CC) aes_decrypt.o aes.o -o aes_decrypt
	
aes_decrypt_cbc: aes_decrypt_cbc.o aes.o aes.h
	$(CC) aes_decrypt_cbc.o aes.o -o aes_decrypt_cbc

check: check.o aes.o 
	$(CC) check.o aes.o -o check

check.o: check.cpp aes.h
	$(CC) -c check.cpp -o check.o

aes.o: aes.cpp aes.h
	$(CC) -c aes.cpp -o aes.o

aes_encrypt.o: aes_encrypt.cpp
	$(CC) -c aes_encrypt.cpp -o aes_encrypt.o

aes_encrypt_cbc.o: aes_encrypt_cbc.cpp
	$(CC) -c aes_encrypt_cbc.cpp -o aes_encrypt_cbc.o
	
aes_decrypt.o: aes_decrypt.cpp
	$(CC) -c aes_decrypt.cpp -o aes_decrypt.o

aes_decrypt_cbc.o: aes_decrypt_cbc.cpp
	$(CC) -c aes_decrypt_cbc.cpp -o aes_decrypt_cbc.o

.PHONY: clean

clean:
	rm -rf *.o check aes_encrypt aes_decrypt


	


