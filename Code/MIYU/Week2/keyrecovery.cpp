#include<iostream>
#include<iomanip>
#include<random>
#include"present.h"

using namespace std;

extern const int ROUND = 5;

const int N = ( 1 << 16 );

struct pcpair 
{
    word8 P0[64];
    word8 P1[64];
    word8 C0[64];
    word8 C1[64];
};


void genAttackPC( vector<pcpair> & PCV, word8 K[80] )
{
    // input difference = 0000f00f00000000
    // output difference = 0000040404040404
    // differential probability = 2^-12

    // random number
    random_device rd;
    mt19937 gen( rd() );
    uniform_int_distribution<word8> bin(0, 1); 

    // generate random key
    for ( int i = 0; i < 80; i++ ) 
        K[i] = bin( gen );

    // initialize the input difference
    unsigned long long XORL = 0x0000f00f00000000ULL;
    word8 XOR[64] = { 0 };
    for ( int i = 0; i < 64; i++ ) 
        XOR[i] = XORL >> i & 0x1;

    // generate random plaintext and encrytpt the pairs ( X, XOR ) 
    for ( int T = 0; T < N; T++ )
    {
        // the first pair
        word8 P0[64] = { 0 }; 
        for ( int i = 0; i < 64; i++ )
            P0[i] = bin( gen );

        word8 P1[64] = { 0 }; 
        for ( int i = 0; i < 64; i++ )
            P1[i] = P0[i] ^ XOR[i];

        struct pcpair pc;
        for ( int i = 0; i < 64; i++ )
        {
            pc.P0[i] = P0[i];
            pc.P1[i] = P1[i];
        }

        Encrypt( P0, K );
        Encrypt( P1, K );

        for ( int i = 0; i < 64; i++ )
        {
            pc.C0[i] = P0[i];
            pc.C1[i] = P1[i];
        }

        PCV.push_back( pc );
    }
}

bool isZero( word8 X0[64], word8 X1[64], int index )
{
    for ( int i = 0; i < 4; i++ )
        if ( X0[4 * index + i] != X1[4 * index + i] )
            return false;
    return true;
}

void keyrecovery()
{
    vector<pcpair> PCV;
    word8 K[80] = { 0 };
    genAttackPC( PCV, K );
    word8 RK[32][64];

    KeySchedule( K, RK );
    InvPL( RK[5] );

    // prepare the data: inverse the ciphertext pairs by InvPL
    for ( auto & it : PCV )
    {
        InvPL( it.C0 );
        InvPL( it.C1 );
    }

    // filter definitely wrong pairs
    vector<pcpair> PCVR;

    for ( auto it : PCV )
    {
        if ( isZero( it.C0, it.C1, 1 ) && 
             isZero( it.C0, it.C1, 3 ) && 
             isZero( it.C0, it.C1, 5 ) && 
             isZero( it.C0, it.C1, 7 ) && 
             isZero( it.C0, it.C1, 9 ) && 
             isZero( it.C0, it.C1, 11 ) && 
             isZero( it.C0, it.C1, 12 ) && 
             isZero( it.C0, it.C1, 13 ) && 
             isZero( it.C0, it.C1, 14 ) && 
             isZero( it.C0, it.C1, 15 ) ) 
        {
            PCVR.push_back( it );
        }
    }

    cout << "After filtering: ";
    cout << dec <<  PCVR.size() << endl;

    // 存储recovery 所有subKey 
    word8 recovered_key[16] = { 0 };
    
    // 恢复第一个subKey  (位置0)
    int counter0[16] = { 0 };
    for ( auto it : PCVR )
    {
        for ( int k0 = 0; k0 < 16; k0++ )
        {
            word8 t0 = ( it.C0[3] << 3 ) ^ ( it.C0[2] << 2 ) ^ ( it.C0[1] << 1 ) ^ ( it.C0[0] ); 
            word8 t1 = ( it.C1[3] << 3 ) ^ ( it.C1[2] << 2 ) ^ ( it.C1[1] << 1 ) ^ ( it.C1[0] ); 

            auto diff = inv_Sbox[ t0 ^ k0 ] ^ inv_Sbox[ t1 ^ k0 ];

            if ( diff == 0x4 )
                counter0[ k0 ] += 1;
        }
    }
    
    // 找出计数最大的subKey 
    int max_count0 = 0;
    word8 likely_key0 = 0;
    for ( int i = 0; i < 16; i++ )
    {
        cout << "subKey 0 bit " << hex << i << " count: " << dec << counter0[i] << endl;
        if (counter0[i] > max_count0) {
            max_count0 = counter0[i];
            likely_key0 = i;
        }
    }
    recovered_key[0] = likely_key0;
    cout << "recovery subKey 0: " << hex << (int)likely_key0 << endl;
    
    // 恢复第二个subKey  (位置2)
    int counter2[16] = { 0 };
    for ( auto it : PCVR )
    {
        for ( int k2 = 0; k2 < 16; k2++ )
        {
            word8 t0 = ( it.C0[11] << 3 ) ^ ( it.C0[10] << 2 ) ^ ( it.C0[9] << 1 ) ^ ( it.C0[8] ); 
            word8 t1 = ( it.C1[11] << 3 ) ^ ( it.C1[10] << 2 ) ^ ( it.C1[9] << 1 ) ^ ( it.C1[8] ); 

            auto diff = inv_Sbox[ t0 ^ k2 ] ^ inv_Sbox[ t1 ^ k2 ];

            if ( diff == 0x4 )
                counter2[ k2 ] += 1;
        }
    }
    
    // 找出计数最大的subKey 
    int max_count2 = 0;
    word8 likely_key2 = 0;
    for ( int i = 0; i < 16; i++ )
    {
        cout << "subKey 2 bit " << hex << i << " count: " << dec << counter2[i] << endl;
        if (counter2[i] > max_count2) {
            max_count2 = counter2[i];
            likely_key2 = i;
        }
    }
    recovered_key[2] = likely_key2;
    cout << "recovery subKey 2: " << hex << (int)likely_key2 << endl;
    
    // 恢复第三个subKey  (位置4)
    int counter4[16] = { 0 };
    for ( auto it : PCVR )
    {
        for ( int k4 = 0; k4 < 16; k4++ )
        {
            word8 t0 = ( it.C0[19] << 3 ) ^ ( it.C0[18] << 2 ) ^ ( it.C0[17] << 1 ) ^ ( it.C0[16] ); 
            word8 t1 = ( it.C1[19] << 3 ) ^ ( it.C1[18] << 2 ) ^ ( it.C1[17] << 1 ) ^ ( it.C1[16] ); 

            auto diff = inv_Sbox[ t0 ^ k4 ] ^ inv_Sbox[ t1 ^ k4 ];

            if ( diff == 0x4 )
                counter4[ k4 ] += 1;
        }
    }
    
    int max_count4 = 0;
    word8 likely_key4 = 0;
    for ( int i = 0; i < 16; i++ )
    {
        cout << "subKey 4 bit " << hex << i << " count: " << dec << counter4[i] << endl;
        if (counter4[i] > max_count4) {
            max_count4 = counter4[i];
            likely_key4 = i;
        }
    }
    recovered_key[4] = likely_key4;
    cout << "recovery subKey 4: " << hex << (int)likely_key4 << endl;
    
    // 恢复第四个subKey  (位置6)
    int counter6[16] = { 0 };
    for ( auto it : PCVR )
    {
        for ( int k6 = 0; k6 < 16; k6++ )
        {
            word8 t0 = ( it.C0[27] << 3 ) ^ ( it.C0[26] << 2 ) ^ ( it.C0[25] << 1 ) ^ ( it.C0[24] ); 
            word8 t1 = ( it.C1[27] << 3 ) ^ ( it.C1[26] << 2 ) ^ ( it.C1[25] << 1 ) ^ ( it.C1[24] ); 

            auto diff = inv_Sbox[ t0 ^ k6 ] ^ inv_Sbox[ t1 ^ k6 ];

            if ( diff == 0x4 )
                counter6[ k6 ] += 1;
        }
    }
    
    int max_count6 = 0;
    word8 likely_key6 = 0;
    for ( int i = 0; i < 16; i++ )
    {
        cout << "subKey 6 bit " << hex << i << " count: " << dec << counter6[i] << endl;
        if (counter6[i] > max_count6) {
            max_count6 = counter6[i];
            likely_key6 = i;
        }
    }
    recovered_key[6] = likely_key6;
    cout << "recovery subKey 6: " << hex << (int)likely_key6 << endl;
    
    // 恢复第五个subKey  (位置8)
    int counter8[16] = { 0 };
    for ( auto it : PCVR )
    {
        for ( int k8 = 0; k8 < 16; k8++ )
        {
            word8 t0 = ( it.C0[35] << 3 ) ^ ( it.C0[34] << 2 ) ^ ( it.C0[33] << 1 ) ^ ( it.C0[32] ); 
            word8 t1 = ( it.C1[35] << 3 ) ^ ( it.C1[34] << 2 ) ^ ( it.C1[33] << 1 ) ^ ( it.C1[32] ); 

            auto diff = inv_Sbox[ t0 ^ k8 ] ^ inv_Sbox[ t1 ^ k8 ];

            if ( diff == 0x4 )
                counter8[ k8 ] += 1;
        }
    }
    
    int max_count8 = 0;
    word8 likely_key8 = 0;
    for ( int i = 0; i < 16; i++ )
    {
        cout << "subKey 8 bit " << hex << i << " count: " << dec << counter8[i] << endl;
        if (counter8[i] > max_count8) {
            max_count8 = counter8[i];
            likely_key8 = i;
        }
    }
    recovered_key[8] = likely_key8;
    cout << "recovery subKey 8: " << hex << (int)likely_key8 << endl;
    
    // 恢复第六个subKey  (位置10)
    int counter10[16] = { 0 };
    for ( auto it : PCVR )
    {
        for ( int k10 = 0; k10 < 16; k10++ )
        {
            word8 t0 = ( it.C0[43] << 3 ) ^ ( it.C0[42] << 2 ) ^ ( it.C0[41] << 1 ) ^ ( it.C0[40] ); 
            word8 t1 = ( it.C1[43] << 3 ) ^ ( it.C1[42] << 2 ) ^ ( it.C1[41] << 1 ) ^ ( it.C1[40] ); 

            auto diff = inv_Sbox[ t0 ^ k10 ] ^ inv_Sbox[ t1 ^ k10 ];

            if ( diff == 0x4 )
                counter10[ k10 ] += 1;
        }
    }
    
    int max_count10 = 0;
    word8 likely_key10 = 0;
    for ( int i = 0; i < 16; i++ )
    {
        cout << "subKey 10 bit " << hex << i << " count: " << dec << counter10[i] << endl;
        if (counter10[i] > max_count10) {
            max_count10 = counter10[i];
            likely_key10 = i;
        }
    }
    recovered_key[10] = likely_key10;
    cout << "recovery subKey 10: " << hex << (int)likely_key10 << endl;
    
    // 输出所有recovery subKey 
    cout << "\n恢复最后一轮密钥 (部分): ";
    for (int i = 0; i < 16; i+=2) {
        if (recovered_key[i] != 0)
            cout << hex << (int)recovered_key[i] << " ";
        else
            cout << "? ";
    }
    cout << endl;
    
    // 输出实际密钥进行比较
    cout << "实际密钥 (前16位): ";
    for (int i = 0; i < 16; i+=2) {
        word8 actual_key = ( RK[5][4*i+3] << 3 ) ^ ( RK[5][4*i+2] << 2 ) ^ ( RK[5][4*i+1] << 1 ) ^ RK[5][4*i];
        cout << hex << (int)actual_key << " ";
    }
    cout << endl;
}

int main()
{
    keyrecovery();
}
