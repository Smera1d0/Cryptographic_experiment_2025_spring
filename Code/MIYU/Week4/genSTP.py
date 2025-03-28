import math
import os
import re

Sbox = [ 0xC, 0x6, 0x9, 0x0, 0x1, 0xA, 0x2, 0xB, 0x3, 0x8, 0x5, 0xD, 0x4, 0xE, 0x7, 0xF ]

def dot( m, x, N = 4 ):
    s = 0
    for i in range( N ):
        s ^= ( m >> i & 1 ) & ( x >> i & 1 ) 
    return s

def genLAT ( Sbox, N = 4 ):
    LAT = [ [0 for i in range(16) ] for j in range(16) ]
    for inmask in range(16):
        for outmask in range(16):
            for x in range(16):
                if dot( inmask, x ) == dot( outmask, Sbox[x] ):
                    LAT[inmask][outmask] += 1
                else:
                    LAT[inmask][outmask] -= 1
    return LAT


def int2bit( x, N = 4 ):
    return '0bin' + bin( x )[2:].zfill( N )

class SKINNYSTP:
    def __init__(self):
        self.var = 0
        self.constrs = []

    def addConstr(self, constr):
        self.constrs.append( constr )
    
    def addVar( self, length = 1 ):
        self.var += 1
        s = 'VV_{}: BITVECTOR({});'.format( self.var, length )
        self.addConstr( s )
        return 'VV_{}'.format( self.var )

    def genModel( self, ROUND, obj ):
        # declare variables 
        X = [ [ self.addVar( length = 4 ) for i in range(16) ] for r in range(ROUND + 1 ) ]
        Y = [ [ self.addVar( length = 4 ) for i in range(16) ] for r in range(ROUND ) ]
        P = [ [ self.addVar( length = 8 ) for i in range(16) ] for r in range(ROUND ) ]

        INL = [ 0x9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xd, 0, 0, 9, 0, 0 ]

        OUTL = [ 0, 0, 0, 0, 0, 0xf, 0, 0, 0, 0xf, 0, 0, 0, 0xf, 0, 0 ]


        # declare LAT
        LAT = genLAT ( Sbox )
        s = 'LAT: ARRAY BITVECTOR(8) OF BITVECTOR(8);'
        self.addConstr( s )
        # declare entries
        for i in range(16):
            for j in range(16):
                if LAT[i][j] != 0:
                    s = 'ASSERT LAT[{}@{}] = {};'.format( int2bit(i), int2bit(j), int2bit( 4 - int( math.log( int( abs( LAT[i][j] ) ), 2 ) ), 8 ) )
                    self.addConstr( s )
                else:
                    s = 'ASSERT LAT[{}@{}] = {};'.format( int2bit(i), int2bit(j), int2bit( 2 ** 8 - 1, 8 ) )
                    self.addConstr( s )

        
        # add constraints
        for r in range( ROUND ):
            # sub nibbles
            for i in range(16):
                s = 'ASSERT LAT[{}@{}] = {};'.format( X[r][i], Y[r][i], P[r][i] )
                self.addConstr( s )
                s = 'ASSERT {} /= {};'.format( P[r][i], int2bit(255, 8) )
                self.addConstr( s )

            # shift rows
            T = [ 0 for i in range(16 ) ]
            T[0] = Y[r][0]
            T[1] = Y[r][1]
            T[2] = Y[r][2]
            T[3] = Y[r][3]
            T[4] = Y[r][7]
            T[5] = Y[r][4]
            T[6] = Y[r][5]
            T[7] = Y[r][6]
            T[8] = Y[r][10]
            T[9] = Y[r][11]
            T[10] = Y[r][8]
            T[11] = Y[r][9]
            T[12] = Y[r][13]
            T[13] = Y[r][14]
            T[14] = Y[r][15]
            T[15] = Y[r][12]

            # mix columns
            # M^t = 1, 1, 0, 1 
            #       0, 0, 1, 0
            #       1, 0, 1, 1
            #       1, 0, 0, 0

            for i in range(4):
                s = 'ASSERT {} = BVXOR( {}, BVXOR({}, {}) );'.format( T[0 + i], X[r+1][0 + i], X[r+1][4 + i], X[r+1][12 + i] )
                self.addConstr( s )
                s = 'ASSERT {} = {};'.format( T[4 + i], X[r+1][8 + i] )
                self.addConstr( s )
                s = 'ASSERT {} = BVXOR( {}, BVXOR({}, {} ) );'.format( T[8 + i], X[r+1][0 + i], X[r+1][8 + i], X[r+1][12 + i] )
                self.addConstr( s )
                s = 'ASSERT {} = {};'.format( T[12 + i], X[r+1][0 + i] )
                self.addConstr( s )

        s = 'ASSERT BVLE( BVPLUS( 8, {} ), {} );'.format(
            ','.join( [ P[i][j] for j in range(16) for i in range( ROUND ) ] ),
            int2bit( obj, 8 )
        )
        self.addConstr( s )

        for i in range(16):
            s = 'ASSERT %s = %s;' % ( X[0][i], int2bit( INL[i] ) )
            self.addConstr( s )

        for i in range(16):
            s = 'ASSERT %s = %s;' % ( X[ROUND][i], int2bit( OUTL[i] ) )
            self.addConstr( s )

        s = 'ASSERT {} /= {};'.format( '@'.join( X[0] ), int2bit( 0, 64 ) )
        self.addConstr( s )

        return X, Y, P

    def solve(self):
        f = open( 'Model.cvc', 'w' )
        f.write( '\n'.join( self.constrs ) ) 
        f.write( '\nQUERY FALSE;' )
        f.write( '\nCOUNTEREXAMPLE;' )
        f.close()

        os.system ( 'stp Model.cvc --cryptominisat --threads 4 > result.txt' )
    
    def parseResult(self, ROUND, X, Y, P):
        pattern = re.compile( r'(VV_\d+) = 0x([0-9A-Fa-f]+)' )
        f = open( 'result.txt', 'r' )
        lines = f.readlines()
        f.close()
        for line in lines:
            match = re.search( pattern, line )
            if match:
                var = match.group(1)
                value = int ( match.group(2), 16 )

                #print( var, value )

                for r in range( ROUND + 1 ):
                    for i in range( 16 ):
                        if var == X[r][i]:
                            X[r][i] = value

                for r in range( ROUND ):
                    for i in range( 16 ):
                        if var == Y[r][i]:
                            Y[r][i] = value

                for r in range( ROUND ):
                    for i in range( 16 ):
                        if var == P[r][i]:
                            P[r][i] = value
        return X, Y, P

if __name__ == '__main__':
    #LAT = genLAT( Sbox )

    #for row in LAT:
    #    print( row )

    ROUND = 3
    OBJ = 5

    skinny = SKINNYSTP( )
    X, Y, P = skinny.genModel( ROUND, OBJ )
    skinny.solve()
    X, Y, P = skinny.parseResult( ROUND, X, Y, P )

    for r in range( ROUND ):
        for i in range(16):
            try:
                print( '{:x}'.format( X[r][i] ), end = '\t' )
            except ValueError as e:
                print( e, X[r][i] )

        print( 'Cor: {}'.format( sum( P[r] ) ) )

        for i in range(16):
            print( '{:x}'.format( Y[r][i] ), end = '\t' )
        print()
        print()


    for i in range(16):
        print( '{:x}'.format( X[ROUND][i] ), end = '\t' )
    print( 'Cor: {}'.format( sum( [ P[i][j] for j in range(16) for i in range(ROUND ) ] ) ) )

    print( ','.join( list( map ( lambda x: hex(x), X[0] ) ) ) )
    print( ','.join( list( map ( lambda x: hex(x), X[ROUND] ) ) ) )

