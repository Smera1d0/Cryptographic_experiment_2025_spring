from gurobipy import *

Sbox = [ 0xC, 0x5, 0x6, 0xB, 0x9, 0x0, 0xA, 0xD, 0x3, 0xE, 0xF, 0x8, 0x4, 0x7, 0x1, 0x2 ] 

def genDDT ( Sbox ):
    DDT = [ [ 0 for i in range(16) ] for j in range(16) ]
    for xx in range(16):
        for x in range(16):
            DDT[xx][ Sbox[x] ^ Sbox[x ^ xx] ] += 1
    return DDT

if __name__ == '__main__':

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

    DDT = genDDT( Sbox )

    for i in range(16):
        for j in range(16):
            if DDT[i][j] == 0:
                a0 = i >> 3 & 0x1
                a1 = i >> 2 & 0x1
                a2 = i >> 1 & 0x1
                a3 = i >> 0 & 0x1

                b0 = j >> 3 & 0x1
                b1 = j >> 2 & 0x1
                b2 = j >> 1 & 0x1
                b3 = j >> 0 & 0x1

                l = 0
                if a0 == 0: l += x0 
                else: l += (1 - x0)

                if a1 == 0: l += x1 
                else: l += (1 - x1)

                if a2 == 0: l += x2 
                else: l += (1 - x2)

                if a3 == 0: l += x3 
                else: l += (1 - x3)

                if b0 == 0: l += y0 
                else: l += (1 - y0)

                if b1 == 0: l += y1 
                else: l += (1 - y1)

                if b2 == 0: l += y2 
                else: l += (1 - y2)

                if b3 == 0: l += y3 
                else: l += (1 - y3)

                m.addConstr( l >= 1 )

    # remove the trivial solution (0,0,0,0) --> (0,0,0,0)
    m.addConstr( x0 + x1 + x2 + x3 + y0 + y1 + y2 + y3 >= 1 )

    m.setObjective( x0 + x1 + x2 + x3 + y0 + y1 + y2 + y3, GRB.MINIMIZE )

    m.optimize()

    if m.status == GRB.OPTIMAL:
        print ( m.getObjective().getValue() )
    else:
        print ( "Error" )










