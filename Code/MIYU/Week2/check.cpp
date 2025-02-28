#include<iostream>
#include"present.h"

using namespace std;

extern const int ROUND = 31;

int main()
{
    word8 X[64] = { 0, 1, 0, 1 };
    word8 XA[64] = {1,1,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1};
    SubNib( X );
    if ( check( X, XA ) ) cout << "SubNib: \t YES" << endl;
    else cout << "SubNib: \t NO" << endl;

    word8 XL[64] = { 0, 1, 0, 1 };
    word8 XLA[64] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    PL( XL );
    if ( check( XL, XLA ) ) cout << "PL: \t YES" << endl;
    else cout << "PL: \t NO" << endl;

    word8 XX[64] = { 0 };
    word8 K[80] = { 0 };

    word8 XXA[64] = { 1,0,1,0,0,0,1,0,0,0,1,0,0,0,0,1,0,1,0,0,0,1,0,0,1,1,0,1,1,1,1,0,0,0,0,1,1,1,0,0,1,0,0,0,0,0,1,1,1,0,0,1,1,1,1,0,1,0,1,0,1,0,1,0 };
    Encrypt( XX, K );

    if ( check( XX, XXA ) ) cout << "Encrypt: \t YES" << endl;
    else cout << "Encrypt: \t NO" << endl;
}


