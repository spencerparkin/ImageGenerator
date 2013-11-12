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
	Sphere* sphere = new Sphere( center, radius, materialProperties );
	sphere->CloneTextures( this );
	return sphere;
}

//===========================================================================
/*virtual*/ bool Sphere::Configure( wxXmlNode* xmlNode )
{
	if( !Object::Configure( xmlNode ) )
		return false;

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

	// The surface normal needs to face the ray.
	surfacePoint.normal = c3ga::unit( surfacePoint.point - center );
	if( quadratic.C < 0.0 )
		surfacePoint.normal = -surfacePoint.normal;
	
	surfacePoint.materialProperties = materialProperties;
	surfacePoint.object = this;
	ApplyTextures( surfacePoint );
	return true;
}

//===========================================================================
/*virtual*/ Scene::Object::Side Sphere::CalculateRaySide( const Scene::Ray& ray ) const
{
	double distance = c3ga::norm( ray.point - center );
	return distance <= radius ? INSIDE : OUTSIDE;
}

//===========================================================================
/*virtual*/ bool Sphere::CalculateTextureCoordinates( const c3ga::vectorE3GA& point, c3ga::vectorE3GA& textureCoordinates ) const
{
	c3ga::vectorE3GA unitSpherePoint = c3ga::unit( point - center );

	double lattitudeAngle = acos( unitSpherePoint.get_e2() );
	double longitudeAngle = atan2( unitSpherePoint.get_e3(), unitSpherePoint.get_e1() );
	if( longitudeAngle < 0.0 )
		longitudeAngle += 2.0 * M_PI;

	textureCoordinates.set( c3ga::vectorE3GA::coord_e1_e2_e3, longitudeAngle / ( 2.0 * M_PI ), lattitudeAngle / M_PI, 0.0 );
	return true;
}

// Sphere.cpp