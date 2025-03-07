from gurobipy import *
import math
from Ineq import Ineq

Sbox = [ 0xC, 0x5, 0x6, 0xB, 0x9, 0x0, 0xA, 0xD, 0x3, 0xE, 0xF, 0x8, 0x4, 0x7, 0x1, 0x2 ] 
Perm = [ 0,16,32,48,1,17,33,49,2,18,34,50,3,19,35,51,4,20,36,52,5,21,37,53,6,22,38,54,7,23,39,55,8,24,40,56,9,25,41,57,10,26,42,58,11,27,43,59,12,28,44,60,13,29,45,61,14,30,46,62,15,31,47, 63 ]

def PassLinear( X ):
    Y = X[:]
    for i in range(64):
        X[ Perm[i] ] = Y[i]

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

def PassSbox( m, X, Y, P ):
    # X[0], X[1], X[2], X[3], Y[0], Y[1], Y[2], Y[3], P[0], P[1]
    L = X + Y + P
    for ineq in Ineq:
        l = ineq[0]
        for i in range(10):
            l += ineq[i+1] * L[i]

        m.addConstr( l >= 0 )

def genModel():
    m = Model() # generate an empty model m

    # add variables to m

    # declare variables - now for 5 states (initial, after round 1, 2, 3, and 4)
    X = [ [ m.addVar( vtype = GRB.BINARY ) for i in range(64) ] for j in range(5) ]
    
    # Probability variables for each round (now 4 rounds)
    P0 = [ [ m.addVar( vtype = GRB.BINARY ) for i in range(16) ] for j in range(4) ]
    P1 = [ [ m.addVar( vtype = GRB.BINARY ) for i in range(16) ] for j in range(4) ]

    # pass the first round function:
    for j in range(16):
        PassSbox( m, X[0][4 * j: 4 * j+4][::-1], X[1][ 4 * j: 4 * j + 4][::-1], [ P0[0][j], P1[0][j] ] )

    # Apply permutation after each round
    temp = X[1][:]
    PassLinear( temp )
    X[1] = temp[:]  

    # pass the second round function:
    for j in range(16):
        PassSbox( m, X[1][4 * j: 4 * j+4][::-1], X[2][ 4 * j: 4 * j + 4][::-1], [ P0[1][j], P1[1][j] ] )
    
    # Apply permutation after round 2
    temp = X[2][:]
    PassLinear( temp )
    X[2] = temp[:] 

    # pass the third round function:
    for j in range(16):
        PassSbox( m, X[2][4 * j: 4 * j+4][::-1], X[3][ 4 * j: 4 * j + 4][::-1], [ P0[2][j], P1[2][j] ] )
    
    # Apply permutation after round 3
    temp = X[3][:]
    PassLinear( temp )
    X[3] = temp[:]

    # pass the fourth round function:
    for j in range(16):
        PassSbox( m, X[3][4 * j: 4 * j+4][::-1], X[4][ 4 * j: 4 * j + 4][::-1], [ P0[3][j], P1[3][j] ] )

    # remove the trivial solution (0,0,0,0) --> (0,0,0,0)
    m.addConstr( sum( X[0] ) >= 1 )

    # Calculate the total probability exponent
    l = 0
    for i in range(4):  # Now for 4 rounds
        for j in range(16):
            l += 2*P0[i][j] + P1[i][j]

    m.setObjective( l, GRB.MINIMIZE )
    m.optimize()

    if m.status == GRB.OPTIMAL:
        print("Optimal objective value:", m.getObjective().getValue())
        
        # Print the differential characteristic
        for r in range(5):  # Print all 5 states (input and after each round)
            print(f"State {r}:", [int(X[r][i].X) for i in range(64)])
        
        # Print probability for each round
        for r in range(4):
            round_prob = 0
            for j in range(16):
                round_prob += 2*P0[r][j].X + P1[r][j].X
            print(f"Round {r+1} probability exponent: {round_prob}")
    else:
        print("Error: No optimal solution found")

if __name__ == '__main__':
    DDT = genDDT( Sbox )
    L = genVectors( DDT )
    genModel()
