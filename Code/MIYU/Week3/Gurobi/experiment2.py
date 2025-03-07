from gurobipy import *
import math
from Ineq import Ineq

Sbox = [ 0xC, 0x5, 0x6, 0xB, 0x9, 0x0, 0xA, 0xD, 0x3, 0xE, 0xF, 0x8, 0x4, 0x7, 0x1, 0x2 ] 

def genDDT ( Sbox ):
    DDT = [ [ 0 for i in range(16) ] for j in range(16) ]
    for xx in range(16):
        for x in range(16):
            DDT[xx][ Sbox[x] ^ Sbox[x ^ xx] ] += 1
    return DDT

def int_2_bits( x, bit = 4 ):
    return list( map ( int, list( bin(x)[2:].zfill( bit ) ) ) )

def genVectors( DDT ):
    V = []
    for i in range(16):
        for j in range(16):
            if DDT[i][j] > 0:
                L = int_2_bits( i ) + int_2_bits( j ) + int_2_bits ( 4 - int( math.log( DDT[i][j], 2 ) ), 2 ) 
                V.append( L )
    return V

def genModel():
    m = Model() # generate an empty model m

    # add varaibles to m

    # input difference
    x0 = m.addVar( vtype = GRB.BINARY ) # x0 is the handle of this variable 
    x1 = m.addVar( vtype = GRB.BINARY ) # x1 is the handle of this variable 
    x2 = m.addVar( vtype = GRB.BINARY ) # x2 is the handle of this variable 
    x3 = m.addVar( vtype = GRB.BINARY ) # x3 is the handle of this variable 

    # output difference
    y0 = m.addVar( vtype = GRB.BINARY ) 
    y1 = m.addVar( vtype = GRB.BINARY ) 
    y2 = m.addVar( vtype = GRB.BINARY ) 
    y3 = m.addVar( vtype = GRB.BINARY ) 

    p0 = m.addVar( vtype = GRB.BINARY )
    p1 = m.addVar( vtype = GRB.BINARY )

    V = [ x0, x1, x2, x3, y0, y1, y2, y3, p0, p1 ]

    for ineq in Ineq:
        l = ineq[0]
        for i in range(10):
            l += ineq[i + 1] * V[i]
        
        m.addConstr( l >= 0 )

    # wt( x ) + wt( y ) == 4
    m.addConstr( x0 + x1 + x2 + x3 + y0 + y1 + y2 + y3 == 4 )

    # remove the trivial solution (0,0,0,0) --> (0,0,0,0)
    m.addConstr( x0 + x1 + x2 + x3 + y0 + y1 + y2 + y3 >= 1 )

    m.setObjective( 2 * p0 + p1, GRB.MINIMIZE )

    m.optimize()

    if m.status == GRB.OPTIMAL:
        print ( m.getObjective().getValue() )
    else:
        print ( "Error" )

if __name__ == '__main__':
    DDT = genDDT( Sbox )
    L = genVectors( DDT )

    #print( L )

    genModel()
    


