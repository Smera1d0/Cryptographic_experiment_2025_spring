#ifndef __CRYPTODEF_H__
#define __CRYPTODEF_H__

#include<iostream>
#include<iomanip>

using namespace std;

typedef unsigned char word8;
typedef unsigned char word4;
typedef unsigned char BIT;

inline ostream & operator << ( ostream & os, word4 x )  
{
    os << setw(1) << setfill('0') << hex << int( x );
    os << dec;
    return os;
}

inline ostream & operator << ( ostream & os, BIT X[16] )  
{
    for ( int i = 0; i < 16; i++ )
        os << setw(1) << hex <<  int( X[i] ) << "";

    os << dec;

    return os;
}

#endif
        
