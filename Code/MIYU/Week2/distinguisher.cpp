#include<iostream>
#include<random>
#include"present.h"

using namespace std;

//extern const int ROUND = 32;
extern const int ROUND = 4;

const int N = ( 1 << 16 );

int main()
{
    // input difference = 0000f00f00000000
    // output difference = 0000040404040404
    // differential probability = 2^-12

    // random number
    random_device rd;
    mt19937 gen( rd() );
    uniform_int_distribution<word8> bin(0, 1); 

    // generate random key
    word8 K[80] = { 0 }; 
    for ( int i = 0; i < 80; i++ ) 
        K[i] = bin( gen );

    // initialize the input difference
    unsigned long long XORL = 0x0000f00f00000000ULL;
    word8 XOR[64] = { 0 };
    for ( int i = 0; i < 64; i++ ) 
        XOR[i] = XORL >> i & 0x1;

    int suc = 0;
    // generate random plaintext and encrytpt the pairs ( X, XOR ) 
    for ( int T = 0; T < N; T++ )
    {
        word8 X[64] = { 0 };
        for ( int i = 0; i < 64; i++ )
            X[i] = bin( gen );
        
        word8 X_prime[64] = { 0 };
        for ( int i = 0; i < 64; i++ )
            X_prime[i] = X[i] ^ XOR[i];
        
        Encrypt(X, K);
        Encrypt(X_prime, K);
        
        unsigned long long diff = 0;
        for ( int i = 0; i < 64; i++ )
            diff ^= (static_cast<unsigned long long>(X[i]^X_prime[i]) << i);
        
        // Check if output difference matches expected difference
        if ( diff == 0x0000040404040404ULL )
            suc += 1;
    }

    cout << "The number of right pairs is " << endl; 
    cout << suc << endl;
    cout << "The expected number is N * 2^-12 = 16" << endl;
}


