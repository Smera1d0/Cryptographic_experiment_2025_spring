from CNF import CNF
from genSboxConstr import *
import sys

Perm = [ 0,16,32,48,1,17,33,49,2,18,34,50,3,19,35,51,4,20,36,52,5,21,37,53,6,22,38,54,7,23,39,55,8,24,40,56,9,25,41,57,10,26,42,58,11,27,43,59,12,28,44,60,13,29,45,61,14,30,46,62,15,31,47, 63 ]

Sbox = [ 0xC, 0x5, 0x6, 0xB, 0x9, 0x0, 0xA, 0xD, 0x3, 0xE, 0xF, 0x8, 0x4, 0x7, 0x1, 0x2 ] 

def PassLinear( X ):
    Y = X[:]
    for i in range(64):
        X[ Perm[i] ] = Y[i]

class PRESENT( CNF ):
    def __init__( self ):
        CNF.__init__( self )
        self.SboxConstr = genSboxConstr( Sbox )

    def genModel( self, obj, Round  ):
        self.reset()

        X = [ [ self.gen_var() for i in range( 64 ) ] for j in range(Round + 1) ]

        P1 = [ [ self.gen_var() for i in range( 16 ) ] for j in range(Round) ]
        P2 = [ [ self.gen_var() for i in range( 16 ) ] for j in range(Round) ]
        P3 = [ [ self.gen_var() for i in range( 16 ) ] for j in range(Round) ]
        
        for r in range(Round):
            for i in range(16):
                L = []
                L += X[r][ 4 * i : 4 * i + 4 ][::-1] 
                L += X[r + 1][ 4 * i : 4 * i + 4 ][::-1] 

                L.append( P1[r][i] ) 
                L.append( P2[r][i] ) 
                L.append( P3[r][i] )

                self.gen_constr_exclude_vector( L, self.SboxConstr )

            if r < Round - 1:
                PassLinear( X[r+1] )

        P = []
        for r in range( Round ):
            for j in range(16):
                P.append( P1[r][j] ) 

        for r in range( Round ):
            for j in range(16):
                P.append( P2[r][j] ) 

        for r in range( Round ):
            for j in range(16):
                P.append( P3[r][j] ) 

        # exclude the trivial result
        self.gen_constr_exclude_vector( X[0], [ ['0'] * 64 ] )

        self.seq_sum( P, obj )

        return X, P1, P2, P3

if __name__ == '__main__':
    Round = 4
    OBJ = 12
    present = PRESENT ()
    cnf = 'present.cnf'  
    X, P1, P2, P3 = present.genModel( OBJ, Round )
    present.printCNF( cnf )

    res_dict = {}
    res = present.runCNF ( cnf, res_dict )

    if res == True:
        print( Round, OBJ, "Yes" )

        vx = [ [0 for i in range(64) ] for j in range(5) ]
        for r in range( Round + 1 ):
            for i in range(64):
                vx[r][i] = res_dict [ X[r][i] ] 

        p1 = [ [0 for i in range(16) ] for j in range(4) ]
        for r in range( Round ):
            for i in range(16):
                p1[r][i] = res_dict [ P1[r][i] ] 

        p2 = [ [0 for i in range(16) ] for j in range(4) ]
        for r in range( Round ):
            for i in range(16):
                p2[r][i] = res_dict [ P2[r][i] ] 

        p3 = [ [0 for i in range(16) ] for j in range(4) ]
        for r in range( Round ):
            for i in range(16):
                p3[r][i] = res_dict [ P3[r][i] ] 

        vxx = [ [ 0 for i in range(16) ] for j in range(5) ]
        for r in range(Round+1):
            for i in range(16):
                vxx[r][i] = ( vx[r][4 * i] << 0 ) ^ ( vx[r][4 * i+1] << 1 ) ^ ( vx[r][4 * i + 2] << 2 ) ^ ( vx[r][4 * i + 3] << 3 )

        ppp = 0
        for r in range(5):
            if r < 4:
                pp = 0
                for j in range(16):
                    print( '%x' % vxx[r][ 15 - j ], end = '\t' )
                    pp += p1[r][j] + p2[r][j] + p3[r][j]

                print( "Pro: 2^-", pp ) 
                ppp += pp

            else:
                for j in range(16):
                    print( '%x' % vxx[r][ 15 - j ], end = '\t' )

                print( "Pro: 2^-", ppp ) 
    else:
        print( Round, OBJ, "No" )

