#ifndef __AES_H__
#define __AES_H__

#include<iostream>
#include<iomanip>

using namespace std;

typedef unsigned char word8;

extern const int ROUND;

extern word8 Sbox[256];

extern word8 inv_Sbox[256];

inline ostream & operator << ( ostream & os, word8 x )  
{
    os << setw(2) << setfill('0') << hex << int( x );
    os << dec;
    return os;
}

inline ostream & operator << ( ostream & os, word8 X[16] )  
{
    for ( int i = 0; i < 16; i++ )
    {
        os << setw(2) << setfill('0') << hex <<  int( X[i] ) << "\t";
        if ( ( ( i + 1 ) % 4 == 0 ) && ( i != 15 )  ) 
            os << endl;
    }
    os << dec;

    return os;
}

inline void printA( word8 X[16] )
{
    cout << "{";
    for ( int i = 0; i < 16; i++ )
    {
        if ( i < 15 )
            cout << hex << "0x" << X[i] << ",";
        else
            cout << hex << "0x" << X[i] << "}";
    }
}

void SubBytes( word8 X[16] );
void InvSubBytes( word8 X[16] );
void ShiftRows( word8 X[16] );
void InvShiftRows( word8 X[16] );
word8 xtime( word8 x );
word8 mul( word8 x, word8 y );
void MixColumns( word8 X[16] );
void InvMixColumns( word8 X[16] );
void AddRoundKey( word8 X[16], word8 K[16] );

void AES_Encrypt( word8 P[16], const word8 K[16] );
void KeyExpansion( const word8 k[16], word8 rk[][16] );
void AES_Decrypt( word8 C[16], const word8 K[16] );
#endif
