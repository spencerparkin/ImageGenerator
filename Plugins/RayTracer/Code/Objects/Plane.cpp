// Plane.cpp

#include "../Header.h"

//===========================================================================
Plane::Plane( void )
{
	center.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 0.0 );
	normal.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 1.0, 0.0 );
}

//===========================================================================
Plane::Plane(
		const c3ga::vectorE3GA& center,
		const c3ga::vectorE3GA& normal,
		const Scene::MaterialProperties& materialProperties ) : Object( materialProperties )
{
	this->center = center;
	this->normal = normal;
}

//===========================================================================
/*virtual*/ Plane::~Plane( void )
{
}

//===========================================================================
/*virtual*/ Scene::Element* Plane::Clone( void ) const
{
	return new Plane( center, normal, materialProperties );
}

//===========================================================================
/*virtual*/ bool Plane::Configure( wxXmlNode* xmlNode )
{
	if( !materialProperties.Configure( Scene::FindNode( xmlNode, "material" ) ) )
		return false;

	center = Scene::LoadVector( xmlNode, "center", center );
	normal = Scene::LoadVector( xmlNode, "normal", normal );
	return true;
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

	if( numerator < 0.0 )
		surfacePoint.normal = normal;
	else
		surfacePoint.normal = -normal;

	surfacePoint.materialProperties = materialProperties;
	return true;
}

// Plane.cpp