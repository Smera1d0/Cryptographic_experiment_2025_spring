#include<iostream>

using namespace std;

unsigned Sbox[16] =    { 0xC, 0x6, 0x9, 0x0, 0x1, 0xA, 0x2, 0xB, 0x3, 0x8, 0x5, 0xD, 0x4, 0xE, 0x7, 0xF };

int dot(unsigned a, unsigned b, int N = 4) {
    int result = 0;
    for (int i = 0; i < N; i++) {
        result ^= ((a >> i) & 1) & ((b >> i) & 1);
    }
    return result;
}

void genLAT(unsigned Sbox[16], int LAT[16][16]) {
    for (int inmask = 0; inmask < 16; inmask++) {
        for (int outmask = 0; outmask < 16; outmask++) {
            LAT[inmask][outmask] = 0;
            for (int x = 0; x < 16; x++) {
                if (dot(inmask, x) == dot(outmask, Sbox[x])) {
                    LAT[inmask][outmask]++;
                } else {
                    LAT[inmask][outmask]--;
                }
            }
        }
    }
}

int main() {
    int LAT[16][16];
    
    genLAT(Sbox, LAT);

    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            cout << static_cast<double>(LAT[i][j]/16.0) << "\t";
        }
        cout << endl;
    }
    
    return 0;
}



