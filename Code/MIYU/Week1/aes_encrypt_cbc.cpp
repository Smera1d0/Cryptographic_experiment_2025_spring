#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include "aes.h"

using namespace std;

extern const int ROUND = 10;

void encryptFile(const string &inputFile, const string &outputFile, const word8 K[16], const word8 IV[16] ) 
{
    // Open the input file for reading
    ifstream inFile(inputFile, ios::binary);
    if (!inFile) {
        cerr << "Error opening input file!" << endl;
        return;
    }

    // Open the output file for writing
    ofstream outFile(outputFile, ios::binary);
    if (!outFile) {
        cerr << "Error opening output file!" << endl;
        return;
    }

    // AES key initialization
    // Read the input file in blocks of 16 bytes (AES block size)
    word8 buffer[16];
    word8 IV_temp[16];
    for ( int i = 0; i < 16; i++ )
        IV_temp[i] = IV[i];

    while ( inFile.read( reinterpret_cast<char*>( buffer ), 16 ) || inFile.gcount() > 0 ) 
    {
        // Padding (if necessary) for the last block
        if ( inFile.gcount() < 16) 
        {
            for (size_t i = inFile.gcount(); i < 16; ++i) 
                buffer[i] = 16 - inFile.gcount();  // Padding byte
        }

        for ( int i = 0; i < 16; i++ )
            buffer[i] ^= IV_temp[i];

        // Encrypt the block
        AES_Encrypt( buffer, K );

        for ( int i = 0; i < 16; i++ )
            IV_temp[i] = buffer[i];

        // Write the encrypted block to the output file
        outFile.write(reinterpret_cast<char*>( buffer ), 16 );
    }

    cout << "File encrypted successfully!" << endl;

    // Close files
    inFile.close();
    outFile.close();
}

int main() {
    // Define a 128-bit key (16 bytes) for AES encryption
    word8 key[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf};


    word8 IV[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

    string inputFile = "sduintro.txt";  // Name of the file to encrypt
    string outputFile = "sduintro_cbc_encrypted.txt";  // Name of the output file

    // Encrypt the file
    encryptFile(inputFile, outputFile, key, IV );

    return 0;
}
