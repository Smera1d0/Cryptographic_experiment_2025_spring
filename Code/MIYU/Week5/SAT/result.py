import re

pattern_sat = re.compile( r's SATISFIABLE' ) 

def parse( filename ):
    Flag = None
    with open( filename, 'r' ) as f:
        line = f.readline().replace( '\n', '' )

        m = re.match( pattern_sat, line )

        if m:
            Flag = True
        else:
            Flag = False

        res_dict = {}

        for line in f:
            if line[0] == 'v':
                X = line[1:].split( )
                V = list ( map( int, X ) )

                for v in V:
                    if v < 0:
                        res_dict [ -v ] = 0
                    elif v > 0:
                        res_dict [ v ] = 1

    return Flag, res_dict 

index = 0
def gen_var():
    global index
    index += 1
    return index

def printPath( filename, Round ):
    X = [ [ gen_var() for i in range( 64 ) ] for j in range(Round) ]
    Y = [ [ gen_var() for i in range( 64 ) ] for j in range(Round) ]
    P1 = [ [gen_var() for i in range( 16 ) ] for j in range(Round) ]
    P2 = [ [gen_var() for i in range( 16 ) ] for j in range(Round) ]
    P3 = [ [gen_var() for i in range( 16 ) ] for j in range(Round) ]

    res_dict = {}

    flag, resdict = parse( filename )

    res_dict.update( resdict ) 

    if flag == True:
        print( "Feasible" )
        for r in range(Round):
            print( "Round ", r + 1 )
            print( "B Sbox:\t", end = '' )
            for i in range( 16 ):
                for j in range(4):
                    print( res_dict[ X[r][ j + 4 * ( 15 - i ) ] ], end = '' )
                print( ' ', end = '' )

            pro = 0
            for i in range(16):
                pro += res_dict[ P1[r][15 - i] ] + res_dict[ P2[r][15- i] ] + res_dict[ P3[r][15 - i] ]

            print( '\t Pro:\t', '2^', pro, end = '' )

            print()


            print( "A Sbox:\t", end = '' )
            for i in range( 16 ):
                for j in range(4):
                    print( res_dict[ Y[r][ j + 4 * ( 15 - i ) ] ], end = '' )
                print( ' ', end = '' )
            print()

        print()
    else:
        print( "Infeasible" )

if __name__ == '__main__': 
    #print( parse( 'res.txt' ) )
    #printPath( "main6_sec_0_res.txt", 3 )
    printPath( "tmp.txt", 3 )
