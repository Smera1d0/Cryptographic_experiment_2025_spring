#include<iostream>
#include<iomanip>
#include<cmath>
#include"present.h"

using namespace std;

word4 Sbox[16] = { 0xC, 0x5, 0x6, 0xB, 0x9, 0x0, 0xA, 0xD, 0x3, 0xE, 0xF, 0x8, 0x4, 0x7, 0x1, 0x2 }; 

word4 inv_Sbox[16] = { 5, 14, 15, 8, 12, 1, 2, 13, 11, 4, 6, 3, 0, 7, 9, 10 };
//permutation layer
int P[64] = { 0,16,32,48,1,17,33,49,2,18,34,50,3,19,35,51,4,20,36,52,5,21,37,53,6,22,38,54,7,23,39,55,8,24,40,56,9,25,41,57,10,26,42,58,11,27,43,59,12,28,44,60,13,29,45,61,14,30,46,62,15,31,47, 63 };

int inv_P[64] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 1, 5, 9, 13, 17, 21, 25, 29, 33, 37, 41, 45, 49, 53, 57, 61, 2, 6, 10, 14, 18, 22, 26, 30, 34, 38, 42, 46, 50, 54, 58, 62, 3, 7, 11, 15, 19, 23, 27, 31, 35, 39, 43, 47, 51, 55, 59, 63 }; 

const int ROUND = 31;

void SubNib( BIT X[64] )
{
    // NOTE !!!
    // the indexes of bits are 
    // 63 62 61 60 ... 3 2 1 0
    for ( int i = 0;i < 16; i++ )
    {
        word4 t = Sbox [ ( X[4*i + 3] << 3 ) ^  
                  ( X[4*i + 2] << 2 ) ^  
                  ( X[4*i + 1] << 1 ) ^  
                  ( X[4*i + 0] << 0 ) ];  

        X[4*i+3] = t >> 3 & 0x1;
        X[4*i+2] = t >> 2 & 0x1;
        X[4*i+1] = t >> 1 & 0x1;
        X[4*i+0] = t >> 0 & 0x1;
    }
}

void InvSubNib( BIT X[64] )
{
    for ( int i = 0;i < 16; i++ )
    {
        word4 t = inv_Sbox [ ( X[4*i + 3] << 3 ) ^  
                  ( X[4*i + 2] << 2 ) ^  
                  ( X[4*i + 1] << 1 ) ^  
                  ( X[4*i + 0] << 0 ) ];  

        X[4*i+3] = t >> 3 & 0x1;
        X[4*i+2] = t >> 2 & 0x1;
        X[4*i+1] = t >> 1 & 0x1;
        X[4*i+0] = t >> 0 & 0x1;
    }
}

// P Layer
void PL( BIT X[64] )
{
    BIT Y[64] = { 0 };
    for ( int i = 0; i < 64; i++ )
        Y[i] = X[i];
    //clont X 

    // do the bit permutation
    for ( int i = 0; i < 64; i++ )
        X[ P[i] ] = Y[i];
}

// InvP Layer
void InvPL( BIT X[64] )
{
    BIT Y[64] = { 0 };
    for ( int i = 0; i < 64; i++ )
        Y[i] = X[i];

    // do the bit permutation
    for ( int i = 0; i < 64; i++ )
        X[i] = Y[ P[i] ];
}

void KeyAdd( BIT X[64], BIT RK[64] )
{
    for ( int i = 0; i < 64; i++ )
        X[i] ^= RK[i];
}

// K is changed in place
void KeySchedule( BIT K[80], BIT RK[][64] )
{
    for ( int r = 0; r < ROUND; r++ ) 
    {
        for ( int i = 0; i < 64; i++ )
        {
            // extract the round key
            RK[r][i] = K[ i + 16 ];
        }

         // update the key
        BIT T[80] = { 0 };
        for ( int i = 0; i < 80; i++ )
            T[i] = K[i];

        for ( int i = 0; i < 80; i++ )
            K[i] = T[ ( i + 19 ) % 80 ];

        // apply Sbox to the leftmost bits
        word4 t = Sbox[ ( K[79] << 3 ) ^ ( K[78] << 2 ) ^ ( K[77] << 1 ) ^ K[76] ]; 
        K[79] = t >> 3 & 0x1;
        K[78] = t >> 2 & 0x1;
        K[77] = t >> 1 & 0x1;
        K[76] = t >> 0 & 0x1;

        // add round counter
        K[19] ^= (r+1) >> 4 & 0x1;
        K[18] ^= (r+1) >> 3 & 0x1;
        K[17] ^= (r+1) >> 2 & 0x1;
        K[16] ^= (r+1) >> 1 & 0x1;
        K[15] ^= (r+1) >> 0 & 0x1;
    }

    // extract the last round key
    for ( int i = 0; i < 64; i++ )
        RK[ROUND][i] = K[ i + 16 ];
}

void Encrypt( BIT X[64], const BIT T[80] )
{
    BIT K[80] = { 0 };

    for ( int i = 0; i < 80; i++ )
        K[i] = T[i];

    BIT RK[ROUND + 1][64];

    KeySchedule( K, RK );

    for ( int i = 0; i < ROUND; i++ )
    {
        KeyAdd( X, RK[i] );
        SubNib( X );
        PL( X );
    }
    KeyAdd( X, RK[ROUND] );
}

void genDDT( word4 S[16], int DDT[16][16] )
{
    for ( int xx = 0; xx < 16; xx++ )
        for ( int x = 0; x < 16; x++ )
        {
            DDT[xx][ S[ x ] ^ S[ x ^ xx ] ] += 1; 
        }

    for ( int i = 0; i < 16; i++ )
        for ( int j = 0; j < 16; j++ )
        {
            if ( DDT[i][j] > 0 )
                DDT[i][j] = 4 - int( log2( DDT[i][j] ) );
            else
                DDT[i][j] = -1;
        }
}






