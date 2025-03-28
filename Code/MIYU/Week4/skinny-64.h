#ifndef __SKINNY_H__
#define __SKINNY_H__

#include"cryptodef.h"

extern word4 Sbox[16]; 
extern word4 invSbox[16];

void SubNibble( word4 X[16] );
void InvSubNibble( word4 X[16] );
void MixColumn( word4 X[16] );
void InvMixColumn( word4 X[16] );
void InvShiftRow( word4 X[16] );
void ShiftRow( word4 X[16] );
void UpdateTweak( word8 TK[16] );
void InvUpdateTweak( word8 TK[16] );
void AddRoundConst( word8 X[16], int r);

void Encrypt( word4 X[16], word4 TK[16] );
void Decrypt( word4 X[16], word4 TK[16] );

extern int ROUND;

#endif
