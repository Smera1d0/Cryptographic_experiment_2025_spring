K = [[0,0,0,1,0,0,0,1]]

vectors = Polyhedron( vertices = K)
INEQ = vectors.inequalities_list()
EQ = vectors.equations_list()

print( INEQ )
print( EQ )