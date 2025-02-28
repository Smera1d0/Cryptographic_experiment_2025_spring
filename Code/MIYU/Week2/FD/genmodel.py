import math

S = [ 0xC, 0x5, 0x6, 0xB, 0x9, 0x0, 0xA, 0xD, 0x3, 0xE, 0xF, 0x8, 0x4, 0x7, 0x1, 0x2 ] 

Perm = [ 0,16,32,48,1,17,33,49,2,18,34,50,3,19,35,51,4,20,36,52,5,21,37,53,6,22,38,54,7,23,39,55,8,24,40,56,9,25,41,57,10,26,42,58,11,27,43,59,12,28,44,60,13,29,45,61,14,30,46,62,15,31,47, 63 ]

cons = []

def genDDT( S ):
    DDT = [ [ 0 for i in range(16) ] for j in range(16) ]
    for xx in range(16):
        for x in range(16):
            DDT[xx][ S[x] ^ S[x^xx] ] += 1

    return DDT

def toBin( x, n ):
    return '0bin' + bin(x)[2:].zfill(n) 

def addConstr( s ):
    cons.append( s )

def declareVar( X, bit = 1 ):
    return '%s : BITVECTOR(%d);' % ( X, bit )

def genModel( Round, obj ):
    # generate variabels
    X = [ [ 0 for i in range(64) ] for r in range( Round + 1 ) ]
    for r in range( Round + 1 ):
        for i in range( 64 ):
            x = 'X_%d_%d' % ( r, i ) 
            addConstr( declareVar( x ) )
            X[r][i] = x

    Y = [ [ 0 for i in range(64) ] for r in range( Round ) ]
    for r in range( Round ):
        for i in range( 64 ):
            x = 'Y_%d_%d' % ( r, i ) 
            addConstr( declareVar( x ) )
            Y[r][i] = x

    # generate variabels
    P = [ [ 0 for i in range(16) ] for r in range(Round ) ]
    for r in range( Round ):
        for i in range( 16 ):
            x = 'P_%d_%d' % ( r, i ) 
            addConstr( declareVar( x, 10 ) )
            P[r][i] = x

    DDT = genDDT ( S )

    # generate DDT
    s = 'DDT: ARRAY BITVECTOR(8) OF BITVECTOR(10);'
    addConstr( s )
    for i in range(16):
        for j in range(16):
            if DDT[i][j] > 0:
                s = 'ASSERT DDT[%s@%s] = %s;' % ( toBin( i, 4 ), toBin( j, 4 ), toBin( int( 4 - math.log( DDT[i][j], 2 ) ), 10 ) )  
                addConstr( s )
            else:
                s = 'ASSERT DDT[%s@%s] = %s;' % ( toBin( i, 4 ), toBin( j, 4 ), '0bin0000000001' )  
                addConstr( s )

    # pass Sbox
    for r in range( Round ):
        for i in range(16):
            s = 'ASSERT DDT[%s@%s@%s@%s@%s@%s@%s@%s] = %s;' % ( X[r][4 * i + 3], 
                                                                X[r][4 * i + 2],  
                                                                X[r][4 * i + 1],  
                                                                X[r][4 * i + 0],  
                                                                Y[r][4 * i + 3], 
                                                                Y[r][4 * i + 2], 
                                                                Y[r][4 * i + 1], 
                                                                Y[r][4 * i + 0], 
                                                                P[r][i] )
            addConstr( s )

            s = 'ASSERT DDT[%s@%s@%s@%s@%s@%s@%s@%s] /= %s;' % ( X[r][4 * i + 3], 
                                                                X[r][4 * i + 2],  
                                                                X[r][4 * i + 1],  
                                                                X[r][4 * i + 0],  
                                                                Y[r][4 * i + 3], 
                                                                Y[r][4 * i + 2], 
                                                                Y[r][4 * i + 1], 
                                                                Y[r][4 * i + 0], 
                                                                toBin(1, 10 ) )
            addConstr( s )

        if r < Round - 1:
            for i in range(64):
                s = 'ASSERT %s = %s;' % ( X[r + 1][ Perm[i] ],  Y[r][i] )
                addConstr( s )


    s = 'ASSERT BVLE( BVPLUS(10 '  
    for r in range(Round):
        for i in range( 16 ):
            s += ', %s' % P[r][i]
    s += '), %s);' % ( toBin(obj, 10) )
    addConstr( s )

    s = 'ASSERT %s /= %s;' % ( '@'.join( X[0] ), toBin(0, 64) )
    addConstr ( s )

    s = 'QUERY FALSE;'
    addConstr( s )
    s = 'COUNTEREXAMPLE;' 
    addConstr( s )

if __name__ == '__main__':
    genModel( 3, 7  )
    print( '\n'.join( cons ) )

    DDT = genDDT ( S )

    #or x in range(16):
    #   print( DDT[x] )





