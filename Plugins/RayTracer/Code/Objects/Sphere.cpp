// Sphere.cpp

#include "../Header.h"

//===========================================================================
Sphere::Sphere( const c3ga::vectorE3GA& center, double radius, const Scene::MaterialProperties& materialProperties ) : Object( materialProperties )
{
	dualSphere = c3ga::no + center + 0.5 * ( c3ga::lc( center, center ) - radius * radius ) * c3ga::ni;
}

//===========================================================================
Sphere::Sphere( const c3ga::dualSphere& dualSphere, const Scene::MaterialProperties& materialProperties ) : Object( materialProperties )
{
	this->dualSphere = dualSphere;
}

//===========================================================================
/*virtual*/ Sphere::~Sphere( void )
{
}

//===========================================================================
/*virtual*/ Scene::Element* Sphere::Clone( void ) const
{
	return new Sphere( dualSphere, materialProperties );
}

//===========================================================================
// This math is over-kill, but it's an alternative to the traditional method.
/*virtual*/ bool Sphere::CalculateSurfacePoint(
							const Scene::Ray& ray,
							Scene::SurfacePoint& surfacePoint ) const
{
	c3ga::trivectorE3GA i( c3ga::trivectorE3GA::coord_e1e2e3, 1.0 );

	c3ga::dualLine dualLine;
	dualLine = ( ray.direction + ( ray.point ^ ray.direction ) * c3ga::ni ) * i;

	// A real dual point-pair is an imaginary direct circle.
	c3ga::circle dualPointPair;
	dualPointPair = dualSphere ^ dualLine;

	c3ga::vectorE3GA normal = ray.direction;
	c3ga::vectorE2GA center;
	center = -normal * ( c3ga::lc( c3ga::noni, dualPointPair ^ ( c3ga::no * c3ga::ni ) ) ) * i;

	c3ga::mv mv = -c3ga::lc( center, center ) + 2.0 * (
					c3ga::lc( c3ga::noni, c3ga::no ^ dualPointPair ) * i +
					c3ga::lc( center, normal ) * center ) * normal;
	double squareRadius = mv.get_scalar();
	
	// If the point-pair was imaginary, then the ray did not intersect the sphere.
	if( squareRadius < 0.0 )
		return false;

	// This works if the dual sphere is always normalized.
	c3ga::vectorE3GA sphereCenter;
	sphereCenter = dualSphere;

	surfacePoint.point = center - normal * sqrt( squareRadius );
	surfacePoint.normal = c3ga::unit( surfacePoint.point - sphereCenter );
	surfacePoint.materialProperties = materialProperties;

	return true;
}

// Sphere.cpp