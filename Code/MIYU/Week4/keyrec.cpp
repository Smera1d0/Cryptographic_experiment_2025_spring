#include"skinny-64.h"
#include<iostream>
#include<random>
#include<cmath>

using namespace std;

int ROUND = 4;

int dot( word4 M[16], word4 X[16] )
{
    int s = 0;
    for ( int i = 0; i < 16; i++ )
        for ( int j = 0; j < 4; j++)
            s ^= ( M[i] & X[i] ) >> j & 0x1;
    return s;
}

int DATA = 16;

vector< pair<word4*, word4*> > CollectData( word4 K[16] )
{
    // generate random plaintexts and key
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<word4> dis(0, 15);

    for ( int i = 0; i < 16; i++ )
        K[i] = dis( gen );

    vector< pair<word4*, word4*> > data;

    for ( int p = 0; p < ( 1 << DATA ); p++ )
    {
        word4 *P = new word4[16];
        for ( int i = 0; i < 16; i++ )
            P[i] = dis(gen);

        word4 *C = new word4[16];
        for ( int i = 0; i < 16; i++ )
            C[i] = P[i];

        Encrypt( C, K );

        data.push_back( make_pair( P, C ) );
    }

    return data;
}

void KeyRecovery( const vector< pair<word4*, word4*> > & data, word4 secretK[16] )
{
    double counter[16] = { 0.0 };

    word4 Min[16] = { 
        0x9,0x0,0x0,0x0,
        0x0,0x0,0x0,0x0,
        0x0,0x0,0xd,0x0,
        0x0,0x9,0x0,0x0
    };

    word4 Mout[16] = {
        0x0,0x0,0x0,0x0,
        0x0,0xf,0x0,0x0,
        0x0,0xf,0x0,0x0,
        0x0,0xf,0x0,0x0 };
    
    for ( int guess = 0; guess < 16; guess++ )
    {
        double cor = 0;
        for ( auto it : data )
        {
            word4 *P = new word4[16];
            for ( int i = 0; i < 16; i++ )
                P[i] = it.first[i];

            word4 *C = new word4[16];
            for ( int i = 0; i < 16; i++ )
                C[i] = it.second[i];

            InvMixColumn( C );
            InvShiftRow( C );

            C[5] ^= guess;

            C[5] = invSbox[ C[5] ];
            C[9] = invSbox[ C[9] ];
            C[13] = invSbox[ C[13] ];

            int c = 0;
            c ^= dot( Min, P );
            c ^= dot( Mout, C );

            if ( c == 0 )
                cor += 1;
            else
                cor -= 1;
                
            delete[] P;
            delete[] C;
        }

        counter[guess] = cor / ( 1 << DATA );
    }

    double max = -1;
    word4 determinekey = 0;

    for ( int i = 0; i < 16; i++ )
    {
        if ( abs( counter[i] ) > max )
        {
            determinekey = i;
            max = abs( counter[i] );
        }

        cout << hex << i << " " << counter[i] << endl;
    }
    std::cout << std::endl;

    cout << "MAX: " << determinekey << " " << max << endl;

    UpdateTweak( secretK );
    UpdateTweak( secretK );
    UpdateTweak( secretK );

    std::cout << "Real: " << secretK[5] << endl;
}

int main()
{
    word4 key[16];
    auto data = CollectData( key );
    KeyRecovery( data, key );
}































































































/*
 *         double cor = 0;
        for ( auto it : data )
        {
            word4 *P = new word4[16];
            for ( int i = 0; i < 16; i++ )
                P[i] = it.first[i];

            word4 *C = new word4[16];
            for ( int i = 0; i < 16; i++ )
                C[i] = it.second[i];

            InvMixColumn( C );
            InvShiftRow( C );

            C[5] ^= guess;

            C[5] = invSbox[ C[5] ];
            C[9] = invSbox[ C[9] ];
            C[13] = invSbox[ C[13] ];

            int c = 0;
            c ^= dot( Min, P );
            c ^= dot( Mout, C );

            if ( c == 0 )
                cor += 1;
            else
                cor -= 1;
        }

        counter[guess] = cor / ( 1 << DATA );
*/

