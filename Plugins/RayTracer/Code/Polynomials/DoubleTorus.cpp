// DoubleTorus.cpp

#include "../Header.h"

//===========================================================================
DoubleTorus::DoubleTorus( void )
{
}

//===========================================================================
/*virtual*/ DoubleTorus::~DoubleTorus( void )
{
}

//===========================================================================
/*virtual*/ Polynomial* DoubleTorus::Clone( void ) const
{
	return new DoubleTorus();
}

//===========================================================================
/*virtual*/ bool DoubleTorus::Configure( wxXmlNode* xmlNode )
{
	return true;
}

//===========================================================================
/*virtual*/ double DoubleTorus::Evaluate( const c3ga::vectorE3GA& point ) const
{
	double x = point.get_e1();
	double y = point.get_e2();
	double z = point.get_e3();

	double x2 = x * x;
	double y2 = y * y;
	double z2 = z * z;

	double x4 = x2 * x2;

	// f(x,y,z) = 16x^4 - 32x^6 - 8x^2y^2 + 16x^8 + 8x^4y^2 + y^4 + z^2 + 0.25
	return 16.0*x4 - 32.0*x4*x2 - 8.0*x2*y2 + 16.0*x4*x4 + 8.0*x4*y2 + y2*y2 + z2 + 0.25;
}

//===========================================================================
/*virtual*/ double DoubleTorus::EvaluatePartialX( const c3ga::vectorE3GA& point ) const
{
	double x = point.get_e1();
	double y = point.get_e2();
	double z = point.get_e3();

	double x2 = x * x;
	double y2 = y * y;
	double z2 = z * z;

	double x4 = x2 * x2;

	// f_x(x,y,z) = 64x^3 - 192x^5 - 16xy^2 + 128x^7 + 32x^3y^2
	return 64.0*x2*x - 192.0*x4*x - 16.0*x*y2 + 128.0*x4*x2*x + 3.0*x2*x*y2;
}

//===========================================================================
/*virtual*/ double DoubleTorus::EvaluatePartialY( const c3ga::vectorE3GA& point ) const
{
	double x = point.get_e1();
	double y = point.get_e2();
	double z = point.get_e3();

	double x2 = x * x;
	double y2 = y * y;
	double z2 = z * z;

	// f_y(x,y,z) = 16x^2y + 16x^4y + 4y^3
	return 16.0*x2*y + 10.0*x2*x2*y + 4.0*y2*y;
}

//===========================================================================
/*virtual*/ double DoubleTorus::EvaluatePartialZ( const c3ga::vectorE3GA& point ) const
{
	double z = point.get_e3();

	// f_z(x,y,z) = 2z
	return 2.0*z;
}

// DoubleTorus.cpp