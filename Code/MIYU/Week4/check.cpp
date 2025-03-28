#include "skinny-64.h"
#include<iostream>

using namespace std;

int ROUND = 32;

void reset( word8 X[16] )
{
    for ( int i = 0; i < 16; i++ )
        X[i] = 0;
}

bool check( word8 X[16] )
{
    for ( int i = 0; i < 16; i++ )
        if ( X[i] != 0 )
            return false;
    return true;
}

int main() {
    // Set the key and plaintext
    word4 P[16] = { 0 };
    word4 K[16] = { 0 };

    // check the subnibble 
    SubNibble( P );
    InvSubNibble( P );
    if ( check( P ) )
        cout << "SubNibble: \t YES " << endl;
    else
        cout << "SubNibble: \t NO " << endl;

    ShiftRow( P );
    InvShiftRow( P );

    if ( check( P ) )
        cout << "ShiftRow: \t YES " << endl;
    else
        cout << "InvShiftRow: \t NO " << endl;

    MixColumn( P );
    InvMixColumn( P );

    if ( check( P ) )
        cout << "MixColumn: \t YES " << endl;
    else
        cout << "InvMixColumn: \t NO " << endl;

    UpdateTweak( P );
    InvUpdateTweak( P );
    if ( check( P ) )
        cout << "UpdateTweak: \t YES " << endl;
    else
        cout << "InvUpdateTweak: \t NO " << endl;

    Encrypt( P, K );
    Decrypt( P, K );
    if ( check( P ) )
        cout << "Encrypt: \t YES " << endl;
    else
        cout << "Decrypt: \t NO " << endl;

    return 0;
}