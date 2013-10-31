// Plane.cpp

#include "../Header.h"

//===========================================================================
Plane::Plane(
		const c3ga::vectorE3GA& center,
		const c3ga::vectorE3GA& normal,
		double maximumDistance,
		const Scene::MaterialProperties& materialProperties ) : Object( materialProperties )
{
	this->center = center;
	this->normal = normal;
	this->maximumDistance = maximumDistance;
}

//===========================================================================
/*virtual*/ Plane::~Plane( void )
{
}

//===========================================================================
/*virtual*/ Scene::Element* Plane::Clone( void ) const
{
	return new Plane( center, normal, maximumDistance, materialProperties );
}

//===========================================================================
/*virtual*/ bool Plane::CalculateSurfacePoint( const Scene::Ray& ray, Scene::SurfacePoint& surfacePoint ) const
{
	double numerator = c3ga::lc( center - ray.point, normal );
	double denominator = c3ga::lc( ray.direction, normal );
	if( denominator == 0.0 )
		return false;

	double lambda = numerator / denominator;
	if( lambda < 0.0 )
		return false;

	surfacePoint.point = ray.CalculateRayPoint( lambda );
	if( maximumDistance >= 0.0 && c3ga::norm( surfacePoint.point - ray.point ) > maximumDistance )
		return false;

	if( numerator < 0.0 )
		surfacePoint.normal = normal;
	else
		surfacePoint.normal = -normal;

	surfacePoint.materialProperties = materialProperties;
	return true;
}

// Plane.cpp