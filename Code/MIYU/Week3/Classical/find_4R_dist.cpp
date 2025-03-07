#include<iostream>
#include<cmath>
#include<vector>

#include"present.h"

using namespace std;

class Path 
{
    public:
        Path()
        {
            P = 0;

            for ( int i = 0; i < 4; i++ ) //初始化4轮的概率
                RoundP[i] = 0;

            X = new word4*[5];
            for ( int i = 0; i < 5; i++ ) //初始化各轮的输入输出
                X[i] = new word4[16];
        }

        Path( const Path& path )
        {
            P = path.P;

            for ( int i = 0; i < 4; i++ )
                RoundP[i] = path.RoundP[i];

            X = new word4*[5];
            for ( int i = 0; i < 5; i++ )
                X[i] = new word4[16];

            for ( int i = 0; i < 5; i++ )
                for ( int j = 0; j < 16; j++ )
                    X[i][j] = path.X[i][j];
        }

        Path& operator=(const Path&  path )
        {
            P = path.P;
            for ( int i = 0; i < 4; i++ )
                RoundP[i] = path.RoundP[i];

            for ( int i = 0; i < 5; i++ )
                for ( int j = 0; j < 16; j++ )
                    X[i][j] = path.X[i][j];

            return *this;
        }

        ~Path()
        {
            for ( int i = 0; i < 5; i++ )
                delete [] X[i];
            delete [] X;
        }

        friend ostream& operator<<(ostream& os, const Path& path) 
        {
            for ( int i = 0; i < 5; i++ )
            {
                if ( i < 4 )
                {
                    for ( int j = 0; j < 16; j++ )
                        os << setw(1) << path.X[i][15 - j] << "\t"; 
                    cout << "Pro: 2^-" << path.RoundP[i] << endl;
                }
                else
                {
                    for ( int j = 0; j < 16; j++ )
                        os << setw(1) << path.X[i][15 - j] << "\t"; 

                    cout << "Total Pro: 2^-" << path.P << endl;
                }
            }

            return os;
        }

        word4 **X;
        int P;
        int RoundP[4]; 
};


// NOTE!!! 
// need to free memories for nodes in vector
vector<word4 *> CartesianProduct( const vector<word4> & V0, 
                           const vector<word4> & V1, 
                           const vector<word4> & V2, 
                           const vector<word4> & V3, 
                           const vector<word4> & V4, 
                           const vector<word4> & V5, 
                           const vector<word4> & V6, 
                           const vector<word4> & V7, 
                           const vector<word4> & V8, 
                           const vector<word4> & V9, 
                           const vector<word4> & V10, 
                           const vector<word4> & V11, 
                           const vector<word4> & V12, 
                           const vector<word4> & V13, 
                           const vector<word4> & V14, 
                           const vector<word4> & V15 )
{
    vector<word4*> Nodes;
    for ( auto const & it0 : V0 )
    for ( auto const & it1 : V1 )
    for ( auto const & it2 : V2 )
    for ( auto const & it3 : V3 )
    for ( auto const & it4 : V4 )
    for ( auto const & it5 : V5 )
    for ( auto const & it6 : V6 )
    for ( auto const & it7 : V7 )
    for ( auto const & it8 : V8 )
    for ( auto const & it9 : V9 )
    for ( auto const & it10 : V10 )
    for ( auto const & it11 : V11 )
    for ( auto const & it12 : V12 )
    for ( auto const & it13 : V13 )
    for ( auto const & it14 : V14 )
    for ( auto const & it15 : V15 )
    {
        word4 * P = new word4[16];
        P[0] = it0;
        P[1] = it1;
        P[2] = it2;
        P[3] = it3;
        P[4] = it4;
        P[5] = it5;
        P[6] = it6;
        P[7] = it7;
        P[8] = it8;
        P[9] = it9;
        P[10] = it10;
        P[11] = it11;
        P[12] = it12;
        P[13] = it13;
        P[14] = it14;
        P[15] = it15;
        Nodes.push_back( P );
    }

    return Nodes;
}

vector<Path> PassRound( const Path & path, int current, int DDT[16][16] ) 
{
    // for each X[current][i], 0 <= i < 16, look up the DDT to generate the new node 
    vector<word4> V[16];

    // for the i-th word in the current layer, find all the possible outputs and put them into V[i]
    // IMPLEMENT HERE

    for (int i = 0; i < 16; i++) {
        word4 input_diff = path.X[current][i];
        for (word4 output_diff = 0; output_diff < 16; output_diff++) {
            if (DDT[input_diff][output_diff] >= 0) {
                V[i].push_back(output_diff);
            }
        }
    }

    // call CartesianProduct to get a vector V containing all possible states for output of current SBox layer
    auto VProduct = CartesianProduct( V[0], V[1], V[2], V[3], V[4], V[5], V[6], V[7], V[8], V[9], V[10], V[11], V[12], V[13], V[14], V[15] );

    // for each element in V, generate the elements for current + 1 layer of a new Path 
    vector<Path> NextPaths;

    for ( auto it : VProduct )
    {
        // newPath is copied from path
        Path newPath { path };

        for ( int i = 0; i < 16; i++ )
        {
            int tp = DDT[ path.X[current][i] ][ it[i] ];

            if ( tp >= 0 )
            {
                newPath.P += tp;
                newPath.RoundP[current] += tp;
            }
        }

        // pass the nodes by Linear Layer
        BIT X[64] = { 0 };
        for ( int i = 0; i < 16; i++ )
        {
            X[4 * i + 0] = it[i] >> 0 & 0x1;
            X[4 * i + 1] = it[i] >> 1 & 0x1;
            X[4 * i + 2] = it[i] >> 2 & 0x1;
            X[4 * i + 3] = it[i] >> 3 & 0x1;
        }

        PL ( X );

        for ( int i = 0; i < 16; i++ )
        {
            word4 t =  ( X[4 * i + 0] << 0 ) 
                     ^ ( X[4 * i + 1] << 1 )
                     ^ ( X[4 * i + 2] << 2 )
                     ^ ( X[4 * i + 3] << 3 );

            newPath.X[ current+1 ][i] = t; 

        }

        NextPaths.push_back( newPath );
    }

    // free the memory
    for ( auto & it : VProduct )
        delete it;

    return NextPaths;
}

int main()
{
    int DDT[16][16] = { 0 };
    genDDT( Sbox, DDT );

    /*
    for ( int i = 0; i < 16; i++ )
    {
        for ( int j = 0; j < 16; j++ )
            cout << DDT[i][j] << " ";
        cout << endl;
    }
    */

    Path I;
    for ( int i = 0; i < 16; i++ ) I.X[0][i] = 0;
    I.X[0][8] = 0xf; I.X[0][11] =  0xf;

    auto V0 = PassRound( I, 0, DDT );

    cout << "Size of V0: " << V0.size() << endl;

    vector<Path> V1;
    for ( auto it : V0 )
    {
        auto tmp = PassRound( it, 1, DDT );
        for ( auto jt : tmp )
            V1.push_back( jt );
    }

    cout << "Size of V1: " << V1.size() << endl;

    vector<Path> V2;
    for ( auto it : V1 )
    {
        auto tmp = PassRound( it, 2, DDT );
        for ( auto jt : tmp )
            V2.push_back( jt );
    }

    cout << "Size of V2: " << V2.size() << endl;

    vector<Path> V3;
    for ( auto it : V2 )
    {
        auto tmp = PassRound( it, 3, DDT );
        for ( auto jt : tmp )
            V3.push_back( jt );
    }

    cout << "Size of V3: " << V3.size() << endl;

    int minv = 100;
    Path bestpath;
    for ( auto it : V3 )
    {
        cout << it << endl;
        if ( it.P < minv )
        {
            minv = it.P;
            bestpath = it;
        }
    }

    cout << "Path Number " << V1.size() << endl;

    cout << "Best Path " << endl;
    cout << bestpath << endl;
}
