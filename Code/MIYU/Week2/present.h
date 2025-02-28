#ifndef __PRESENT_H__
#define __PRESENT_H__

#include<iostream>
#include<iomanip>

using namespace std;

typedef unsigned char word8;

extern const int ROUND;

extern word8 Sbox[16];
extern word8 inv_Sbox[16];

extern word8 P[64]; 
extern word8 inv_P[64]; 

inline ostream & operator << ( ostream & os, word8 x )  
{
    os << setw(1) << setfill('0') << hex << int( x );
    os << dec;
    return os;
}

inline ostream & operator << ( ostream & os, word8 X[] )  
{
    int size = 64;

    for ( int i = 0; i < size / 4; i++ )
    {
        word8 t = ( X[4 * ( 15 - i ) + 3 ] << 3 ) ^ 
                  ( X[4 * ( 15 - i ) + 2 ] << 2 ) ^ 
                  ( X[4 * ( 15 - i ) + 1 ] << 1 ) ^ 
                  ( X[4 * ( 15 - i ) + 0 ] << 0 ); 
        
        os << setw(1) << setfill('0') << hex <<  int( t ) << "";
    }
    os << dec;

    return os;
}
inline void printKey( word8 X[80] )
{
    int size = 80;

    for ( int i = 0; i < size / 4; i++ )
    {
        word8 t = ( X[4 * ( 19 - i ) + 3 ] << 3 ) ^ 
                  ( X[4 * ( 19 - i ) + 2 ] << 2 ) ^ 
                  ( X[4 * ( 19 - i ) + 1 ] << 1 ) ^ 
                  ( X[4 * ( 19 - i ) + 0 ] << 0 ); 
        
        cout << setw(1) << setfill('0') << hex << int( t ) << "";
    }
    cout << dec << endl;
}


inline void printA( word8 X[64] )
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

inline bool check ( word8 X[64], word8 answer[64] )
{
    for ( int i = 0; i < 64; i++ )
        if ( X[i] != answer[i] ) 
            return false;
    return true;
}

void SubNib( word8 X[64] );
void InvSubNib( word8 X[64] );
void PL( word8 X[64] );
void KeyAdd( word8 X[64], word8 RK[64] );
void KeySchedule( word8 K[80], word8 RK[][64] );
void Encrypt( word8 X[64], const word8 K[80] );
void InvSubNib( word8 X[64] );
void InvPL( word8 X[64] );

#endif
