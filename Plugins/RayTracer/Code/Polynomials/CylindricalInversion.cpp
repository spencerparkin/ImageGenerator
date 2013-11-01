// CylindricalInversion.cpp

#include "../Header.h"

//===========================================================================
CylindricalInversion::CylindricalInversion( void )
{
	A = 1.0;
}

//===========================================================================
/*virtual*/ CylindricalInversion::~CylindricalInversion( void )
{
}

//===========================================================================
/*virtual*/ Polynomial* CylindricalInversion::Clone( void ) const
{
	CylindricalInversion* cylindricalInversion = new CylindricalInversion();
	cylindricalInversion->A = A;
	return cylindricalInversion;
}

//===========================================================================
/*virtual*/ bool CylindricalInversion::Configure( wxXmlNode* xmlNode )
{
	A = Scene::LoadNumber( xmlNode, "A", A );
	return true;
}

//===========================================================================
/*virtual*/ double CylindricalInversion::Evaluate( const c3ga::vectorE3GA& point ) const
{
	double x = point.get_e1();
	double y = point.get_e2();
	double z = point.get_e3();

	double x2 = x * x;
	double y2 = y * y;
	double z2 = z * z;

	double x4 = x2 * x2;
	double y4 = y2 * y2;
	double z4 = z2 * z2;

	return x4 + y4 + z4 + A*x*( x2 + y2 + z2 ) + 2.0*A*( x2 + z2 ) + 2.0*x2*( y2 + z2 );
}

//===========================================================================
/*virtual*/ double CylindricalInversion::EvaluatePartialX( const c3ga::vectorE3GA& point ) const
{
	double x = point.get_e1();
	double y = point.get_e2();
	double z = point.get_e3();

	double x2 = x * x;
	double y2 = y * y;
	double z2 = z * z;

	return 4.0*x2*x + 2.0*A*x2 + A*y2 + A*z2 + 4.0*A*x + 4.0*x*( y2 + z2 );
}

//===========================================================================
/*virtual*/ double CylindricalInversion::EvaluatePartialY( const c3ga::vectorE3GA& point ) const
{
	double x = point.get_e1();
	double y = point.get_e2();
	double z = point.get_e3();

	return 4.0*y*y*y + 2.0*A*x*y + 4.0*x*x*y;
}

//===========================================================================
/*virtual*/ double CylindricalInversion::EvaluatePartialZ( const c3ga::vectorE3GA& point ) const
{
	double x = point.get_e1();
	double y = point.get_e2();
	double z = point.get_e3();

	return 4.0*z*z*z + 2.0*A*x*z + 4.0*x*x*z;
}

// CylindricalInversion.cpp