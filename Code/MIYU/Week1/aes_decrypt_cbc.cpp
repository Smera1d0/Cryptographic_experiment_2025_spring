#include <iostream>
#include <fstream>
#include <vector>
#include "aes.h"

using namespace std;

extern const int ROUND = 10;

void decryptFile(const string &inputFile, const string &outputFile, const word8 K[16], const word8 IV[16]) 
{
    // Open the encrypted file for reading
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

    word8 buffer[16];
    word8 prevBlock[16];
    word8 IV_temp[16];
    for (int i = 0; i < 16; i++)
        IV_temp[i] = IV[i];

    while (inFile.read(reinterpret_cast<char*>(buffer), 16)) 
    {
        // Save current ciphertext block
        for (int i = 0; i < 16; i++)
            prevBlock[i] = buffer[i];

        // Decrypt the block
        AES_Decrypt(buffer, K);

        // XOR with previous ciphertext block or IV
        for (int i = 0; i < 16; i++) {
            buffer[i] ^= IV_temp[i];
            IV_temp[i] = prevBlock[i];
        }

        // For the last block, remove padding
        if (inFile.peek() == EOF) {
            int paddingLen = buffer[15];
            if (paddingLen > 0 && paddingLen <= 16) {
                outFile.write(reinterpret_cast<char*>(buffer), 16 - paddingLen);
            }
        } else {
            outFile.write(reinterpret_cast<char*>(buffer), 16);
        }
    }

    cout << "File decrypted successfully!" << endl;

    inFile.close();
    outFile.close();
}

int main() {
    // Define a 128-bit key (16 bytes) for AES encryption
    word8 key[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf};

    word8 IV[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

    string inputFile = "sduintro_cbc_encrypted.txt";  // Name of the file to encrypt
    string outputFile = "sduintro_cbc_decrypted.txt";  // Name of the output file

    // Encrypt the file
    decryptFile(inputFile, outputFile, key, IV );

    return 0;
}
