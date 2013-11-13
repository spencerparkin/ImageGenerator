// Torus.cpp

#include "../Header.h"

//===========================================================================
Torus::Torus( void )
{
}

//===========================================================================
/*virtual*/ Torus::~Torus( void )
{
}

//===========================================================================
/*virtual*/ Polynomial* Torus::Clone( void ) const
{
	return new Torus();
}

//===========================================================================
/*virtual*/ bool Torus::Configure( wxXmlNode* xmlNode )
{
	return true;
}

//===========================================================================
/*virtual*/ double Torus::Evaluate( const c3ga::vectorE3GA& point ) const
{
	double x = point.get_e1();
	double y = point.get_e2();
	double z = point.get_e3();

	double x2 = x * x;
	double y2 = y * y;
	double z2 = z * z;

	// f(x,y,z) = (x^2 + y^2 + z^2 + 3)^2 - 16(x^2 + y^2)
	double q = x2 + y2 + z2 + 3.0;
	return q * q - 16.0 * ( x2 + y2 );
}

//===========================================================================
/*virtual*/ double Torus::EvaluatePartialX( const c3ga::vectorE3GA& point ) const
{
	double x = point.get_e1();
	double y = point.get_e2();
	double z = point.get_e3();

	double x2 = x * x;
	double y2 = y * y;
	double z2 = z * z;

	// f_x(x,y,z) = 2(x^2 + y^2 + z^2 + 3)(2x) - 32x = 4(x^2 + y^2 + z^2 - 5)x
	return 4.0 * ( x2 + y2 + z2 - 5.0 ) * x;
}

//===========================================================================
/*virtual*/ double Torus::EvaluatePartialY( const c3ga::vectorE3GA& point ) const
{
	double x = point.get_e1();
	double y = point.get_e2();
	double z = point.get_e3();

	double x2 = x * x;
	double y2 = y * y;
	double z2 = z * z;

	// f_y(x,y,z) = 2(x^2 + y^2 + z^2 + 3)(2y) - 32y = 4(x^2 + y^2 + z^2 - 5)y
	return 4.0 * ( x2 + y2 + z2 - 5.0 ) * y;
}

//===========================================================================
/*virtual*/ double Torus::EvaluatePartialZ( const c3ga::vectorE3GA& point ) const
{
	double x = point.get_e1();
	double y = point.get_e2();
	double z = point.get_e3();

	double x2 = x * x;
	double y2 = y * y;
	double z2 = z * z;

	// f_z(x,y,z) = 2(x^2 + y^2 + z^2 + 3)(2z) = 4(x^2 + y^2 + z^2 + 3)z
	return 4.0 * ( x2 + y2 + 2 + 3.0 ) * z;
}

// Torus.cpp