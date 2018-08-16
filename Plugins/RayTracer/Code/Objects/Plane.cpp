// Plane.cpp

#include "Plane.h"

//===========================================================================
Plane::Plane( void )
{
	center.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 0.0 );
	normal.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 1.0, 0.0 );
	textureScale = 1.0;
}

//===========================================================================
Plane::Plane(
		const c3ga::vectorE3GA& center,
		const c3ga::vectorE3GA& normal,
		const Scene::MaterialProperties& materialProperties ) : Object( materialProperties )
{
	this->center = center;
	this->normal = normal;
	textureScale = 1.0;
}

//===========================================================================
/*virtual*/ Plane::~Plane( void )
{
}

//===========================================================================
/*virtual*/ Scene::Element* Plane::Clone( void ) const
{
	Plane* plane = new Plane( center, normal, materialProperties );
	plane->CloneTextures( this );
	plane->textureScale = textureScale;
	return plane;
}

//===========================================================================
/*virtual*/ bool Plane::Configure( wxXmlNode* xmlNode )
{
	if( !Object::Configure( xmlNode ) )
		return false;

	if( !materialProperties.Configure( Scene::FindNode( xmlNode, "material" ) ) )
		return false;

	center = Scene::LoadVector( xmlNode, "center", center );
	normal = Scene::LoadVector( xmlNode, "normal", normal );

	textureScale = Scene::LoadNumber( xmlNode, "textureScale", textureScale );

	return true;
}

//===========================================================================
/*virtual*/ bool Plane::CalculateSurfacePoint( const Scene::Ray& ray, const Scene& scene, Scene::SurfacePoint& surfacePoint ) const
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
	ApplyTextures( surfacePoint, scene.Eye() );
	return true;
}

//===========================================================================
// A coordinate system is layed out on the plane and the texture is
// scaled into the first quadrant of that system.  UV coordintes
// will go outside of the range [0,1]x[0,1] if the given surface point
// is not in the square of this first quadrant.
/*virtual*/ bool Plane::CalculateTextureCoordinates( const c3ga::vectorE3GA& point, c3ga::vectorE3GA& textureCoordinates ) const
{
	c3ga::bivectorE3GA plane = c3ga::gp( normal, c3ga::I3 );

	c3ga::vectorE3GA uAxis, vAxis;

	uAxis.set( c3ga::vectorE3GA::coord_e1_e2_e3, 1.0, 0.0, 0.0 );
	uAxis = c3ga::lc( c3ga::lc( uAxis, plane ), c3ga::reverse( plane ) );
	if( c3ga::norm( uAxis ) == 0.0 )
	{
		uAxis.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 1.0, 0.0 );
		uAxis = c3ga::lc( c3ga::lc( uAxis, plane ), c3ga::reverse( plane ) );
		if( c3ga::norm( uAxis ) == 0.0 )
		{
			uAxis.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 1.0 );
			uAxis = c3ga::lc( c3ga::lc( uAxis, plane ), c3ga::reverse( plane ) );
			if( c3ga::norm( uAxis ) == 0.0 )
				return false;
		}
	}

	uAxis = c3ga::unit( uAxis );
	vAxis = c3ga::gp( c3ga::op( normal, uAxis ), c3ga::I3 );

	c3ga::vectorE3GA vector = point - center;

	// This works, because the UV axes are orthonormal.
	double u = c3ga::lc( vector, uAxis ) / textureScale;
	double v = c3ga::lc( vector, vAxis ) / textureScale;

	textureCoordinates.set( c3ga::vectorE3GA::coord_e1_e2_e3, u, v, 0.0 );
	return true;
}

// Plane.cpp