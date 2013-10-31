// Quadratic.cpp

#include "Header.h"

//===========================================================================
Quadratic::Quadratic( void )
{
	A = B = C = 1.0;
}

//===========================================================================
Quadratic::Quadratic( double A, double B, double C )
{
	this->A = A;
	this->B = B;
	this->C = C;
}

//===========================================================================
int Quadratic::Solve( double* realRoots ) const
{
	double descriminant = B * B - 4.0 * A * C;
	if( descriminant < 0.0 )
		return 0;

	if( descriminant == 0.0 )
	{
		realRoots[0] = -0.5 * B / A;
		return 1;
	}

	// Notice that in the case of 2 real roots we
	// guarentee here that realRoots[0] < realRoots[1].
	double sqrtDescriminant = sqrt( descriminant );
	realRoots[0] = ( -B - sqrtDescriminant ) * 0.5 / A;
	realRoots[1] = ( -B + sqrtDescriminant ) * 0.5 / A;
	return 2;
}

// Quadratic.cpp