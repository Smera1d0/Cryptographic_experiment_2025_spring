

# This file was *autogenerated* from the file experiment1_sagemath.sage
from sage.all_cmdline import *   # import sage library

_sage_const_0 = Integer(0); _sage_const_1 = Integer(1)
K = [[_sage_const_0 ,_sage_const_0 ,_sage_const_0 ,_sage_const_1 ,_sage_const_0 ,_sage_const_0 ,_sage_const_0 ,_sage_const_1 ]]

vectors = Polyhedron( vertices = K)
INEQ = vectors.inequalities_list()
EQ = vectors.equations_list()

print( INEQ )
print( EQ )

