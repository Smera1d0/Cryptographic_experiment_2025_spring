#ifndef __PRESENT_H__
#define __PRESENT_H__

#include<iostream>
#include<iomanip>

using namespace std;

typedef unsigned char word4;
typedef unsigned char BIT;

extern const int ROUND;

extern word4 Sbox[16];
extern word4 inv_Sbox[16];

extern int P[64]; 
extern int inv_P[64]; 

inline ostream & operator << ( ostream & os, word4 x )  
{
    os << setw(1) << setfill('0') << hex << int( x );
    os << dec;
    return os;
}

inline ostream & operator << ( ostream & os, BIT X[] )  
{
    int size = 64;

    for ( int i = 0; i < size / 4; i++ )
    {
        word4 t = ( X[4 * ( 15 - i ) + 3 ] << 3 ) ^ 
                  ( X[4 * ( 15 - i ) + 2 ] << 2 ) ^ 
                  ( X[4 * ( 15 - i ) + 1 ] << 1 ) ^ 
                  ( X[4 * ( 15 - i ) + 0 ] << 0 ); 
        
        os << setw(1) << setfill('0') << hex <<  int( t ) << "";
    }
    os << dec;

    return os;
}

inline void printKey( BIT X[80] )
{
    int size = 80;

    for ( int i = 0; i < size / 4; i++ )
    {
        word4 t = ( X[4 * ( 19 - i ) + 3 ] << 3 ) ^ 
                  ( X[4 * ( 19 - i ) + 2 ] << 2 ) ^ 
                  ( X[4 * ( 19 - i ) + 1 ] << 1 ) ^ 
                  ( X[4 * ( 19 - i ) + 0 ] << 0 ); 
        
        cout << setw(1) << setfill('0') << hex << int( t ) << "";
    }
    cout << dec << endl;
}


inline void printA( BIT X[64] )
{
    cout << "{";
    for ( int i = 0; i < 64; i++ )
    {
        if ( i < 63 )
            cout << hex << X[i] << ",";
        else
            cout << hex << X[i] << "}";
    }
    cout << endl;
}

inline bool check ( BIT X[64], BIT answer[64] )
{
    for ( int i = 0; i < 64; i++ )
        if ( X[i] != answer[i] ) 
            return false;
    return true;
}

void SubNib( BIT X[64] );
void InvSubNib( BIT X[64] );
void PL( BIT X[64] );
void KeyAdd( BIT X[64], BIT RK[64] );
void KeySchedule( BIT K[80], BIT RK[][64] );
void Encrypt( BIT X[64], const BIT K[80] );
void InvSubNib( BIT X[64] );
void InvPL( BIT X[64] );
void genDDT( word4 S[16], int DDT[16][16] );

#endif
