// Fractal.cpp

#include "Fractal.h"

//===========================================================================
Fractal::Fractal( void )
{
}

//===========================================================================
/*virtual*/ Fractal::~Fractal( void )
{
}

//===========================================================================
bool Fractal::RayMarch( const Scene::Ray& ray, c3ga::vectorE3GA& contactPoint, int maxIteration, double eps /*= 1e-2*/ ) const
{
	Scene::Ray marchingRay = ray;

	for( int i = 0; i < maxIteration; i++ )
	{
		double distance = DistanceEstimate( marchingRay.point );
		marchingRay.point = marchingRay.CalculateRayPoint( distance );
		if( distance < eps )
		{
			contactPoint = marchingRay.point;
			return true;
		}
	}

	return false;
}

// Fractal.cpp