import os 
import re

def genDDT( Sbox ):
    table = [ [ 0 for i in range(16) ] for j in range(16) ]
    for xorin in range(16):
        for x in range(16):
            table[ xorin ] [ Sbox[x] ^ Sbox[x ^ xorin] ] += 1

    return table

def int_2_bin( x, n = 4 ):
    B = []
    for i in range(n):
        B.append( x >> ( n - 1 - i ) & 0x1 )
    return B
    
def genVectorDiff( table ):
    # pro = 0, pro = -2 pro = -3 
    # in a total, 4 + 4 + 3 bits
    V = []
    for i in range(16):
        B0 = int_2_bin( i )
        for j in range(16):
            B1 = int_2_bin( j )

            B = []
            if table[i][j] == 16:
                B = B0 + B1 + [0,0,0]
            elif table[i][j] == 4:
                B = B0 + B1 + [1,1,0]
            elif table[i][j] == 2:
                B = B0 + B1 + [1,1,1]

            if len(B) > 0:
                V.append( B )
    return V

def genCompVectors( vec, dim ):
    com = []
    for x in range( 1 << dim ):
        v = int_2_bin( x, dim )
        if v not in vec:
            com.append( v )
    return com

def genConstr( L ): 
    '''
    L is the set of vectors that need to be excluded
    '''
    filename = 'presentsbox.txt' 

    with open( filename, 'w' ) as f:
        print( '.i %d' % len( L[0] ), file = f )
        print( '.o 1 ', file = f )
        print( '.p %d ' % len( L ), file = f )

        for l in L:
            for ll in l:
                print( ll, end = '', file = f )
            print( ' 1', file = f )

        print( '.e', file = f )

    os.system( 'espresso %s > %s.res' % ( filename, filename ) )

    L = []

    pattern = re.compile( r'([01-]+) 1' )
    P = []
    with open( '%s.res' % filename, 'r' ) as f:
        lines = f.readlines()
        for line in lines:
            m = re.match( pattern, line )
            if m:
                P.append( list ( m.group( 1 ) ) )

    #os.system( 'rm %s' % filename )
    #os.system( 'rm %s.res' % filename )
    return P

def genSboxConstr( sbox ):
    table = genDDT ( sbox )
    vec = genVectorDiff( table )
    com = genCompVectors( vec, len( vec[0] ) )
    P = genConstr( com )
    return P

if __name__ == '__main__':
    Sbox = [ 0xC, 0x5, 0x6, 0xB, 0x9, 0x0, 0xA, 0xD, 0x3, 0xE, 0xF, 0x8, 0x4, 0x7, 0x1, 0x2 ] 
    P = genSboxConstr( Sbox )

    print ( P )

