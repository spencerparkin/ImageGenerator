// Polynomial.cpp

#include "Polynomial.h"

//===========================================================================
Polynomial::Polynomial( void )
{
}

//===========================================================================
/*virtual*/ Polynomial::~Polynomial( void )
{
}

//===========================================================================
/*virtual*/ bool Polynomial::Configure( wxXmlNode* xmlNode )
{
	return true;
}

//===========================================================================
/*virtual*/ double Polynomial::EvaluatePartialX( const c3ga::vectorE3GA& point ) const
{
	double delta = 1e-6;
	c3ga::vectorE3GA forward( c3ga::vectorE3GA::coord_e1_e2_e3, point.get_e1() + delta, point.get_e2(), point.get_e3() );
	c3ga::vectorE3GA backward( c3ga::vectorE3GA::coord_e1_e2_e3, point.get_e1() - delta, point.get_e2(), point.get_e3() );
	return ( Evaluate( forward ) - Evaluate( backward ) ) * 0.5 * delta;
}

//===========================================================================
/*virtual*/ double Polynomial::EvaluatePartialY( const c3ga::vectorE3GA& point ) const
{
	double delta = 1e-6;
	c3ga::vectorE3GA forward( c3ga::vectorE3GA::coord_e1_e2_e3, point.get_e1(), point.get_e2() + delta, point.get_e3() );
	c3ga::vectorE3GA backward( c3ga::vectorE3GA::coord_e1_e2_e3, point.get_e1(), point.get_e2() - delta, point.get_e3() );
	return ( Evaluate( forward ) - Evaluate( backward ) ) * 0.5 * delta;
}

//===========================================================================
/*virtual*/ double Polynomial::EvaluatePartialZ( const c3ga::vectorE3GA& point ) const
{
	double delta = 1e-6;
	c3ga::vectorE3GA forward( c3ga::vectorE3GA::coord_e1_e2_e3, point.get_e1(), point.get_e2(), point.get_e3() + delta );
	c3ga::vectorE3GA backward( c3ga::vectorE3GA::coord_e1_e2_e3, point.get_e1(), point.get_e2(), point.get_e3() - delta );
	return ( Evaluate( forward ) - Evaluate( backward ) ) * 0.5 * delta;
}

//===========================================================================
/*virtual*/ void Polynomial::EvaluateGradient( const c3ga::vectorE3GA& point, c3ga::vectorE3GA& gradient ) const
{
	gradient.set( c3ga::vectorE3GA::coord_e1_e2_e3,
									EvaluatePartialX( point ),
									EvaluatePartialY( point ),
									EvaluatePartialZ( point ) );
}

// Polynomial.cpp