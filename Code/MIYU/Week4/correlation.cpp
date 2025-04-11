#include"skinny-64.h"
#include<iostream>
#include<random>
#include<cmath>

using namespace std;

int ROUND = 3;

int dot( word4 M[16], word4 X[16] )
{
    int s = 0;
    for ( int i = 0; i < 16; i++ )
        for ( int j = 0; j < 4; j++)
            s ^= ( M[i] & X[i] ) >> j & 0x1;
    return s;
}

int DATA = 16;

int main()
{
    // generate random plaintexts and key
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<word4> dis(0, 15);

    word8 K[16] = { 0 };
    for ( int i = 0; i < 16; i++ )
        K[i] = dis( gen );

    //word4 Min[16] = { 0, 0, 0xf, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2, 0x0, 0x0,  0x0, 0x0, 0x0, 0x0, 0x2 };
    //word4 Mout[16] = { 0, 0, 1, 0, 0, 0, 0, 0, 0,  0, 0,  0,  0, 0, 0, 0 };

    word4 Min[16] = { 
    //    0x6,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x4,0x0,0x0,0x4,0x0,0x0};
        0x9,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xd,0x0,0x0,0x9,0x0,0x0
    };
    word4 Mout[16] = {
        //0x0,0x0,0x0,0x0,0x0,0x8,0x0,0x0,0x0,0x8,0x0,0x0,0x0,0x8,0x0,0x0 };
        0x0,0x0,0x0,0x0,0x0,0xf,0x0,0x0,0x0,0xf,0x0,0x0,0x0,0xf,0x0,0x0 };

    double cor = 0;
    for ( int p = 0; p < ( 1 << DATA ); p++ )
    {
        int c = 0;

        word4 *P = new word4[16];

        for ( int i = 0; i < 16; i++ )
            P[i] = dis(gen);
        
        // check the correlation between the input and output of the MixColumn operation
        
        int input_bit = dot(Min, P);

        word4 C[16];
        Encrypt(P,K);
        for (int i = 0; i < 16; i++)
            C[i] = P[i]; 

        int output_bit = dot(Mout, C);

        if (input_bit == output_bit)
            c = 1;
        else
            c = -1; 

        // Accumulate correlation
        cor += c;
        delete [] P;
    }

    cout << ( cor/( 1 << DATA ) ) << endl;
    cout << (log2(abs(cor/(1<<DATA)))) << endl;
}

