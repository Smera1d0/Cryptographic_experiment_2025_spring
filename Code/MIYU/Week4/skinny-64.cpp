#include<iostream>
#include"cryptodef.h"
#include"skinny-64.h"

using namespace std;

word4 Sbox[16] =    { 0xC, 0x6, 0x9, 0x0, 0x1, 0xA, 0x2, 0xB, 0x3, 0x8, 0x5, 0xD, 0x4, 0xE, 0x7, 0xF };
word4 invSbox[16] = { 0x3, 0x4, 0x6, 0x8, 0xC, 0xA, 0x1, 0xE, 0x9, 0x2, 0x5, 0x7, 0x0, 0xB, 0xD, 0xF };

word8 constants[] = { 
0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3E, 0x3D, 0x3B, 0x37, 0x2F, 0x1E, 0x3C, 0x39, 0x33, 0x27, 0x0E,
0x1D, 0x3A, 0x35, 0x2B, 0x16, 0x2C, 0x18, 0x30, 0x21, 0x02, 0x05, 0x0B, 0x17, 0x2E, 0x1C, 0x38,
0x31, 0x23, 0x06, 0x0D, 0x1B, 0x36, 0x2D, 0x1A, 0x34, 0x29, 0x12, 0x24, 0x08, 0x11, 0x22, 0x04,
0x09, 0x13, 0x26, 0x0C, 0x19, 0x32, 0x25, 0x0A, 0x15, 0x2A, 0x14, 0x28, 0x10, 0x20 };

int PT[] = {9,15,8,13,10,14,12,11,0,1,2,3,4,5,6,7};

void SubNibble( word4 X[16] )
{
    for ( int i = 0; i < 16; i++ )
        X[i] = Sbox[ X[i] ];
}

void InvSubNibble( word4 X[16] )
{
    for ( int i = 0; i < 16; i++ )
        X[i] = invSbox[ X[i] ];
}

void AddRoundConst( word4 X[16], int round )
{
    word4 c0 = constants[ round ] & 0x0F;
    word4 c1 = ( constants[ round ] >> 4 ) & 0x3;
    X[0] ^= c0;
    X[4] ^= c1;
    X[8] ^= 0x2;
}

void AddRoundTweakey( word4 X[16], word4 TK[16] )
{
    for ( int i = 0; i < 8; i++ )
        X[i] ^= TK[i];
}

void ShiftRow( word4 X[16] )
{
    word4 T[16] = { 0 };
    for ( int i = 0; i < 16; i++ )
        T[i] = X[i];

    X[4] = T[7];
    X[5] = T[4];
    X[6] = T[5];
    X[7] = T[6];

    X[8] = T[10];
    X[9] = T[11];
    X[10] = T[8];
    X[11] = T[9];

    X[12] = T[13];
    X[13] = T[14];
    X[14] = T[15];
    X[15] = T[12];
}

void InvShiftRow( word4 X[16] )
{
    word4 T[16] = { 0 };
    for ( int i = 0; i < 16; i++ )
        T[i] = X[i];

    X[4] = T[5];
    X[5] = T[6];
    X[6] = T[7];
    X[7] = T[4];

    X[8] = T[10];
    X[9] = T[11];
    X[10] = T[8];
    X[11] = T[9];

    X[12] = T[15];
    X[13] = T[12];
    X[14] = T[13];
    X[15] = T[14];
}


void MixColumn( word4 X[16] )
{
    word8 T[16] = { 0 };
    for ( int i = 0; i < 16; i++ )
        T[i] = X[i];

    for ( int i = 0; i < 4; i++ )
    {
        X[0 + i] = T[0 + i] ^ T[8 + i] ^ T[12 + i];
        X[4 + i] = T[0 + i];
        X[8 + i] = T[4 + i] ^ T[8 + i];
        X[12 + i] = T[0 + i] ^ T[8 + i];
    }
}

void InvMixColumn( word4 X[16] )
{
    word8 T[16] = { 0 };
    for ( int i = 0; i < 16; i++ )
        T[i] = X[i];

    for ( int i = 0; i < 4; i++ )
    {
        X[0 + i] = T[4 + i]; 
        X[4 + i] = T[4 + i] ^ T[8 + i] ^ T[12 + i]; 
        X[8 + i] = T[4 + i] ^ T[12 + i]; 
        X[12 + i] = T[0 + i] ^ T[12 + i];
    }
}

void UpdateTweak( word8 TK[16] ) 
{
    word8 Temp[16] = { 0 };
    for ( int i = 0; i < 16; i++ )
        Temp[i] = TK[i];
    
    for ( int i = 0; i < 16; i++ )
        TK[i] = Temp[ PT[i] ];
}

void InvUpdateTweak( word8 TK[16] ) 
{
    word8 Temp[16] = { 0 };
    for ( int i = 0; i < 16; i++ )
        Temp[i] = TK[i];
    
    for ( int i = 0; i < 16; i++ )
        TK[ PT[i] ] = Temp[i];
}

void Encrypt( word4 X[16], word4 K[16] )
{
    word4 TK[16] = { 0 };
    for ( int i = 0; i < 16; i++ ) 
        TK[i] = K[i];
    
    for ( int r = 0; r < ROUND; r++ )
    {
        SubNibble( X );
        AddRoundConst( X, r );

        AddRoundTweakey( X, TK );
        UpdateTweak( TK );

        ShiftRow( X );
        MixColumn( X );
    }
}

void Decrypt( word4 X[16], word4 K[16] )
{
    word4 TK[16] = { 0 };
    for ( int i = 0; i < 16; i++ ) 
        TK[i] = K[i];

    for( int i = 0; i < ROUND; i++ )
        UpdateTweak( TK );

    for ( int r = ROUND - 1; r >= 0; r--)
    {
        InvMixColumn( X );
        InvShiftRow( X );

        InvUpdateTweak( TK );
        AddRoundTweakey( X, TK );

        AddRoundConst( X, r );
        InvSubNibble( X );
    }
}






