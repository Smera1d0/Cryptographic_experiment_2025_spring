import sys
import re

f = open( sys.argv[1], 'r' )

lines = f.readlines()

f.close()

V = [ [ 0 for i in range(64) ] for r in range(4) ]

for line in lines:
    if 'X_0_' in line:
        line = line.replace (';', '' ).replace( ')', '' ).replace ( 'ASSERT(', '' )
        X = line.split( '=' )
        index = int( X[0].split( '_' )[2] )
        value = int( X[1], 2 )

        V[0][index] = value

for line in lines:
    if 'X_1_' in line:
        line = line.replace (';', '' ).replace( ')', '' ).replace ( 'ASSERT(', '' )
        X = line.split( '=' )
        index = int( X[0].split( '_' )[2] )
        value = int( X[1], 2 )

        V[1][index] = value

for line in lines:
    if 'X_2_' in line:
        line = line.replace (';', '' ).replace( ')', '' ).replace ( 'ASSERT(', '' )
        X = line.split( '=' )
        index = int( X[0].split( '_' )[2] )
        value = int( X[1], 2 )

        V[2][index] = value

for line in lines:
    if 'X_3_' in line:
        line = line.replace (';', '' ).replace( ')', '' ).replace ( 'ASSERT(', '' )
        X = line.split( '=' )
        index = int( X[0].split( '_' )[2] )
        value = int( X[1], 2 )

        V[3][index] = value


VV = [ [ 0 for i in range(16) ] for j in range(4) ]
for i in range(4):
    for j in range(16):
        VV[i][j] = ( V[i][4 * j + 3 ] << 3 ) ^   \
                   ( V[i][4 * j + 2 ] << 2 ) ^   \
                   ( V[i][4 * j + 1 ] << 1 ) ^   \
                   ( V[i][4 * j + 0 ] << 0 )

VY = [ [ 0 for i in range(64) ] for r in range(4) ]
for line in lines:
    if 'Y_0_' in line:
        line = line.replace (';', '' ).replace( ')', '' ).replace ( 'ASSERT(', '' )
        X = line.split( '=' )
        index = int( X[0].split( '_' )[2] )
        value = int( X[1], 2 )

        VY[0][index] = value

for line in lines:
    if 'Y_1_' in line:
        line = line.replace (';', '' ).replace( ')', '' ).replace ( 'ASSERT(', '' )
        X = line.split( '=' )
        index = int( X[0].split( '_' )[2] )
        value = int( X[1], 2 )

        VY[1][index] = value

for line in lines:
    if 'Y_2_' in line:
        line = line.replace (';', '' ).replace( ')', '' ).replace ( 'ASSERT(', '' )
        X = line.split( '=' )
        index = int( X[0].split( '_' )[2] )
        value = int( X[1], 2 )

        VY[2][index] = value

for line in lines:
    if 'Y_3_' in line:
        line = line.replace (';', '' ).replace( ')', '' ).replace ( 'ASSERT(', '' )
        X = line.split( '=' )
        index = int( X[0].split( '_' )[2] )
        value = int( X[1], 2 )

        VY[3][index] = value

VV = [ [ 0 for i in range(16) ] for j in range(5) ]
for i in range(4):
    for j in range(16):
        VV[i][j] = ( V[i][4 * j + 3 ] << 3 ) ^   \
                   ( V[i][4 * j + 2 ] << 2 ) ^   \
                   ( V[i][4 * j + 1 ] << 1 ) ^   \
                   ( V[i][4 * j + 0 ] << 0 )

YVV = [ [ 0 for i in range(16) ] for j in range(5) ]
for i in range(4):
    for j in range(16):
        YVV[i][j] = ( VY[i][4 * j + 3 ] << 3 ) ^   \
                   ( VY[i][4 * j + 2 ] << 2 ) ^   \
                   ( VY[i][4 * j + 1 ] << 1 ) ^   \
                   ( VY[i][4 * j + 0 ] << 0 )

for i in range(4):
    for j in range(16):
        print( VV[i][15 - j], end = ' ' )
    print()

    for j in range(16):
        print( YVV[i][15 - j], end = ' ' )
    print()


    print()
