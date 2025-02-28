#include<iostream>
#include"aes.h"

using namespace std;

extern const int ROUND = 10;

bool check ( word8 X[16], word8 answer[18] )
{
    for ( int i = 0; i < 16; i++ )
        if ( X[i] != answer[i] ) 
            return false;
    return true;
}

int main()
{
    // test SubByte
    word8 SBX[16] = { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf  };
    word8 SBA[16] = {0x63,0x7c,0x77,0x7b,0xf2,0x6b,0x6f,0xc5,0x30,0x01,0x67,0x2b,0xfe,0xd7,0xab,0x76};

    SubBytes( SBX );

    if ( check( SBX, SBA ) ) cout << "SubByte:\t YES" << endl; 
    else cout << "SubByte:\t NO" << endl;

    // test ShiftRow
    word8 SRX[16] = { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf  };
    word8 SRA[16] = { 0x00,0x01,0x02,0x03,0x05,0x06,0x07,0x04,0x0b,0x0c,0x08,0x0a,0x00,0x0d,0x0e,0x0f};
    ShiftRows( SRX );
    
    if ( check( SRX, SRA ) ) cout << "ShiftRow:\t YES" << endl; 
    else cout << "ShiftRow:\t NO" << endl;

    // test xtime
    word8 x = 0x80;
    if ( xtime( x ) == 0x1b ) cout << "xtime:\t YES" << endl; 
    else cout << "xtime:\t NO" << endl; 

    // test mul
    word8 a = 0x02, b = 0x05;
    if ( mul( a, b ) == 0x0a ) cout << "mul:\t YES" << endl; 
    else cout << "mul:\t NO" << endl; 

    // test MixColumn
    word8 MCX[16] = { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf  };
    word8 MCA[16] = {0x09,0x09,0x0a,0x03,0x1d,0x1b,0x1c,0x19,0x03,0x02,0x03,0x1c,0x16,0x10,0x15,0x0e};

    MixColumns( MCX );
    
    if ( check( MCX, MCA ) ) cout << "MixColumn:\t YES" << endl; 
    else cout << "MixColumn:\t NO" << endl;

    // test AddRoundKey
    word8 ARKX[16] = { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf  };
    word8 ARKK[16] = { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf  };

    word8 ARKA[16] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

    AddRoundKey( ARKX, ARKK );
    
    if ( check( ARKA, ARKX ) ) cout << "AddRoundKey:\t YES" << endl; 
    else cout << "AddRoundKey:\t NO" << endl; 

    // test keyexpansion
    word8 K[16] = { 
        0x2b, 0x28, 0xab, 0x09, 
        0x7e, 0xae, 0xf7, 0xcf, 
        0x15, 0xd2, 0x15, 0x4f,
        0x16, 0xa6, 0x88, 0x3c
    };

    word8 KA[16] = {0xd0,0xc9,0xe1,0xb6,0x14,0xee,0x3f,0x63,0xf9,0x25,0x0c,0x0c,0xa8,0x89,0xc8,0xa6};

    word8 RK[ROUND+1][16];

    KeyExpansion( K, RK );

    if ( check( RK[ROUND], KA ) ) cout << "KeyExpansion:\t YES" << endl;
    else cout << "KeyExpansion:\t NO" << endl;

    // test Encryption
    word8 P[16] = { 
        0x32, 0x88, 0x31, 0xe0,  
        0x43, 0x5a, 0x31, 0x37, 
        0xf6, 0x30, 0x98, 0x07,
        0xa8, 0x8d, 0xa2, 0x34 };

    word8 C[16] = {0x39,0x02,0xdc,0x19,0x25,0xdc,0x11,0x6a,0x84,0x09,0x85,0x0b,0x1d,0xfb,0x97,0x32};

    //cout << P << endl;

    AES_Encrypt( P, K );

    if ( check( P, C ) ) cout << "AES_Encrypt:\t YES" << endl;
    else cout << "AES_Encrypt:\t NO" << endl;
}
