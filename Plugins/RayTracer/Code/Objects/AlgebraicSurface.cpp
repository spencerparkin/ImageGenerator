// AlgebraicSurface.cpp

#include "../Header.h"

//===========================================================================
AlgebraicSurface::AlgebraicSurface( void )
{
	polynomial = 0;
}

//===========================================================================
AlgebraicSurface::AlgebraicSurface( Polynomial* polynomial )
{
	SetPolynomial( polynomial );
}

//===========================================================================
/*virtual*/ AlgebraicSurface::~AlgebraicSurface( void )
{
	delete polynomial;
}

//===========================================================================
/*virtual*/ Scene::Element* AlgebraicSurface::Clone( void ) const
{
	AlgebraicSurface* algebraicSurface = new AlgebraicSurface();
	algebraicSurface->polynomial = polynomial ? polynomial->Clone() : 0;
	return algebraicSurface;
}

//===========================================================================
/*virtual*/ bool AlgebraicSurface::Configure( wxXmlNode* xmlNode )
{
	if( polynomial )
		return polynomial->Configure( xmlNode );
	return false;
}

//===========================================================================
/*virtual*/ bool AlgebraicSurface::CalculateSurfacePoint( const Scene::Ray& ray, Scene::SurfacePoint& surfacePoint ) const
{
	double lambda;
	if( !FindRayHit( ray, lambda ) )
		return false;

	surfacePoint.point = ray.CalculateRayPoint( lambda );
	polynomial->EvaluateGradient( surfacePoint.point, surfacePoint.normal );
	surfacePoint.normal = c3ga::unit( surfacePoint.normal );

	c3ga::vectorE3GA point = surfacePoint.point + surfacePoint.normal;
	if( ( polynomial->Evaluate( point ) < 0.0 ) != ( polynomial->Evaluate( ray.point ) < 0.0 ) )
		surfacePoint.normal = -surfacePoint.normal;

	surfacePoint.materialProperties = materialProperties;
	return true;
}

//===========================================================================
// This is an interesting dynamic programming problem, and my initial
// solution here is probably not going to be the best by far.
// The biggest problem with this that I currently see is that we may
// not be finding the root we want.  We want the root that is closest to
// the ray origin point.  Starting with a smaller initial step size may
// help, but is not a very good solution, especially for surfaces that
// loop around very thin in places.
bool AlgebraicSurface::FindRayHit( const Scene::Ray& ray, double& lambda ) const
{
	lambda = 0.0;
	double epsilon = 1e-9;
	int maxIterations = 400;
	double result = 0.0, lastResult = 0.0;
	double delta = 1.0;

	for( int iteration = 0; iteration < maxIterations; iteration++ )
	{
		c3ga::vectorE3GA point = ray.CalculateRayPoint( lambda );
		result = polynomial->Evaluate( point );
		if( fabs( result ) < epsilon )
			return true;
		
		if( lastResult != 0.0 )
		{
			if( fabs( result ) > fabs( lastResult ) )
				delta *= -0.5;
		}
		
		lastResult = result;
		lambda += delta;
	}

	return false;
}

//===========================================================================
void AlgebraicSurface::SetPolynomial( Polynomial* polynomial )
{
	this->polynomial = polynomial;
}

//===========================================================================
Polynomial* AlgebraicSurface::GetPolynomial( void )
{
	return polynomial;
}

//===========================================================================
const Polynomial* AlgebraicSurface::GetPolynomial( void ) const
{
	return polynomial;
}

// AlgebraicSurface.cpp