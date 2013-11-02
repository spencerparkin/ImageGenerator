// Quadric.cpp

#include "../Header.h"

//===========================================================================
Quadric::Quadric( void )
{
	quadraticCoeficients.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 0.0 );
	nonLinearCoeficients.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 0.0 );
	linearCoeficients.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 0.0 );
	constantCoeficient = 0.0;
}

//===========================================================================
/*virtual*/ Quadric::~Quadric( void )
{
}

//===========================================================================
/*virtual*/ Scene::Element* Quadric::Clone( void ) const
{
	Quadric* quadric = new Quadric();

	quadric->quadraticCoeficients = quadraticCoeficients;
	quadric->nonLinearCoeficients = nonLinearCoeficients;
	quadric->linearCoeficients = linearCoeficients;
	quadric->constantCoeficient = constantCoeficient;
	quadric->materialProperties = materialProperties;

	return quadric;
}

//===========================================================================
/*virtual*/ bool Quadric::Configure( wxXmlNode* xmlNode )
{
	if( !materialProperties.Configure( Scene::FindNode( xmlNode, "material" ) ) )
		return false;

	quadraticCoeficients = Scene::LoadVector( xmlNode, "quadratic", quadraticCoeficients );
	nonLinearCoeficients = Scene::LoadVector( xmlNode, "nonLinear", nonLinearCoeficients );
	linearCoeficients = Scene::LoadVector( xmlNode, "linear", linearCoeficients );
	constantCoeficient = Scene::LoadNumber( xmlNode, "constant", constantCoeficient );
	return true;
}

//===========================================================================
/*virtual*/ bool Quadric::CalculateSurfacePoint( const Scene::Ray& ray, Scene::SurfacePoint& surfacePoint, double minimumDistance ) const
{
	Quadratic quadratic;
	quadratic.A = EvaluateQuadratic( ray.direction, false );
	quadratic.B = c3ga::lc( ray.direction, EvaluateQuadraticGradient( ray.point ) );
	quadratic.C = EvaluateQuadratic( ray.point );

	double realRoots[2];
	int realSolutionCount = quadratic.Solve( realRoots );
	if( realSolutionCount == 0 )
		return false;

	// The positive roots are those that the ray can actually see.
	// Root zero is always guarenteed to be less than root one.
	double lambda = realRoots[0];
	if( lambda < 0.0 )
	{
		lambda = realRoots[1];
		if( lambda < 0.0 )
			return false;
	}

	surfacePoint.point = ray.CalculateRayPoint( lambda );
	surfacePoint.normal = c3ga::unit( EvaluateQuadraticGradient( surfacePoint.point ) );
	if( c3ga::lc( ray.direction, surfacePoint.normal ) > 0.0 )
		surfacePoint.normal = -surfacePoint.normal;
	surfacePoint.materialProperties = materialProperties;
	return true;
}

//===========================================================================
double Quadric::EvaluateQuadratic( const c3ga::vectorE3GA& point, bool homogeneous /*= true*/ ) const
{
	double result =
			quadraticCoeficients.get_e1() * point.get_e1() * point.get_e1() +
			quadraticCoeficients.get_e2() * point.get_e2() * point.get_e2() +
			quadraticCoeficients.get_e3() * point.get_e3() * point.get_e3() +
			nonLinearCoeficients.get_e1() * point.get_e2() * point.get_e3() +
			nonLinearCoeficients.get_e2() * point.get_e3() * point.get_e1() +
			nonLinearCoeficients.get_e3() * point.get_e1() * point.get_e2();

	if( homogeneous )
		result +=
			linearCoeficients.get_e1() * point.get_e1() +
			linearCoeficients.get_e2() * point.get_e2() +
			linearCoeficients.get_e3() * point.get_e3() +
			constantCoeficient;

	return result;
}

//===========================================================================
c3ga::vectorE3GA Quadric::EvaluateQuadraticGradient( const c3ga::vectorE3GA& point ) const
{
	c3ga::vectorE3GA gradient( c3ga::vectorE3GA::coord_e1_e2_e3,
			2.0 * quadraticCoeficients.get_e1() * point.get_e1() +
			nonLinearCoeficients.get_e2() * point.get_e3() +
			nonLinearCoeficients.get_e3() * point.get_e2() +
			linearCoeficients.get_e1(),
			2.0 * quadraticCoeficients.get_e2() * point.get_e2() +
			nonLinearCoeficients.get_e1() * point.get_e3() +
			nonLinearCoeficients.get_e3() * point.get_e1() +
			linearCoeficients.get_e2(),
			2.0 * quadraticCoeficients.get_e3() * point.get_e3() +
			nonLinearCoeficients.get_e1() * point.get_e2() +
			nonLinearCoeficients.get_e2() * point.get_e1() +
			linearCoeficients.get_e3() );

	return gradient;
}	

// Quadric.cpp