// Sphere.cpp

#include "../Header.h"

//===========================================================================
Sphere::Sphere( void )
{
	center.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 0.0 );
	radius = 1.0;
}

//===========================================================================
Sphere::Sphere( const c3ga::vectorE3GA& center, double radius, const Scene::MaterialProperties& materialProperties ) : Object( materialProperties )
{
	this->center = center;
	this->radius = radius;
}

//===========================================================================
/*virtual*/ Sphere::~Sphere( void )
{
}

//===========================================================================
/*virtual*/ Scene::Element* Sphere::Clone( void ) const
{
	return new Sphere( center, radius, materialProperties );
}

//===========================================================================
/*virtual*/ bool Sphere::Configure( wxXmlNode* xmlNode )
{
	if( !materialProperties.Configure( Scene::FindNode( xmlNode, "material" ) ) )
		return false;

	center = Scene::LoadVector( xmlNode, "center", center );
	radius = Scene::LoadNumber( xmlNode, "radius", radius );
	return true;
}

//===========================================================================
// I originally implemented this using a CGA dual sphere, intersecting it
// with a CGA dual line to get a dual point-pair, but it's just too slow.
/*virtual*/ bool Sphere::CalculateSurfacePoint(
							const Scene::Ray& ray,
							Scene::SurfacePoint& surfacePoint ) const
{
	c3ga::vectorE3GA delta = ray.point - center;
	Quadratic quadratic;
	quadratic.A = 1.0;		// The square norm of the ray direction should be one.
	quadratic.B = 2.0 * c3ga::lc( delta, ray.direction );
	quadratic.C = c3ga::norm2( delta ) - radius * radius;

	double realRoots[2];
	int realSolutionCount = quadratic.Solve( realRoots );
	if( realSolutionCount == 0 )
		return false;

	// Choose the closer of the two points.
	double lambda = realRoots[0];

	// Can the ray see the point?
	if( lambda < 0.0 )
		return false;		// No.

	surfacePoint.point = ray.CalculateRayPoint( lambda );

	// The surface normal needs to face the ray.
	surfacePoint.normal = c3ga::unit( surfacePoint.point - center );
	if( quadratic.C < 0.0 )
		surfacePoint.normal = -surfacePoint.normal;
	
	surfacePoint.materialProperties = materialProperties;
	return true;
}

// Sphere.cpp