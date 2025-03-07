from Ineq import Ineq
from experiment2 import *

def dot( ineq, x ):
    l = ineq[0]
    for i in range(10):
        l += ineq[i+1] * x[i]
    return l

def greedy( Ineq, Sbar ): 
    # satisfy: v * ineq >= 0 
    # not satisfy: v * ineq < 0
    IneqSimple = []

    while len( Sbar ) > 0: 
        maxv = 0
        maxIneq = None
        for ineq in Ineq:
            # count the number of not-satisfied vectors 
            n = 0
            for x in Sbar:
                if dot( ineq, x ) < 0:
                    n += 1
            if n > maxv:
                maxv = n
                maxIneq = ineq

        Ineq.remove ( maxIneq )
        IneqSimple.append( maxIneq )

        for x in Sbar:
            if dot( maxIneq, x ) < 0:
                Sbar.remove ( x )

    return IneqSimple 

if __name__ == '__main__':
    DDT = genDDT( Sbox )
    S = genVectors( DDT )
    Sbar = []
    for i in range( 1 << 10 ):
        x = int_2_bits( i, 10 )
        if x not in S:
            Sbar.append( x )

    SI = greedy( Ineq, Sbar )

    print( SI )






