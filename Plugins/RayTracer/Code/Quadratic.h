// Quadratic.h

//===========================================================================
// Ax^2 + Bx + C
class Quadratic
{
public:
	Quadratic( void );
	Quadratic( double A, double B, double C );
	int Solve( double* realRoots ) const;
	double A, B, C;
};

// Quadratic.h