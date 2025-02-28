// 生成 present 的差分分布表
#include<iostream>
#include "present.h"

using namespace std;

word8 Sbox[16] = { 0xC, 0x5, 0x6, 0xB, 0x9, 0x0, 0xA, 0xD, 0x3, 0xE, 0xF, 0x8, 0x4, 0x7, 0x1, 0x2 }; 


int main() {
    word8 T[16][16]={0};
    for(int i = 0 ; i < 16 ; i++){
        for(int j = 0 ; j < 16 ; j++){
            word8 in_diff = i;
            word8 out_diff = Sbox[j] ^ Sbox[j^i];
            T[in_diff][out_diff] +=1;
        }
    }

    for (int row = 0 ; row < 16; row ++){
        for (int col = 0;col < 16; col ++){
            cout<<T[row][col] / 16.0 <<"\t";
        }
        cout<<endl;
    }
    return 0;
}