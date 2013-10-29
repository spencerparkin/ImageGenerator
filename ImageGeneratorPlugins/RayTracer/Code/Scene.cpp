// Scene.cpp

#include "Header.h"

//===========================================================================
Scene::Scene( void )
{
}

//===========================================================================
Scene::~Scene( void )
{
}

//===========================================================================
// Return the color that is visible in the scene at the given point and
// looking in the given direction.
void Scene::CalculateVisibleColor( const Ray& ray, c3ga::vectorE3GA& visibleColor ) const
{
	visibleColor.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 0.0 );

	SurfacePoint surfacePoint;
	if( CalculateSurfacePoint( ray, surfacePoint ) )
	{
		c3ga::vectorE3GA color;
		CalculateSurfacePointColor( ray, surfacePoint, color );

		c3ga::vectorE3GA light;
		CalculateSurfacePointIllumination( surfacePoint, light );

		visibleColor.set( c3ga::vectorE3GA::coord_e1_e2_e3,
									color.get_e1() * light.get_e1(),
									color.get_e2() * light.get_e2(),
									color.get_e3() * light.get_e3() );
	}
}

//===========================================================================
// Return the surface characteristics at the point that is found on the
// surface that can be seen by the given ray.  If no surface is seen from
// the given ray, false is returned.
bool Scene::CalculateSurfacePoint( const Ray& ray, SurfacePoint& surfacePoint ) const
{
	SurfacePoint nearestSurfacePoint;
	double smallestDistance = -1.0;

	// Perform a linear search.  If this becomes too slow, we may
	// need to use some sort of spacial sorting data-structure.
	for( ObjectList::const_iterator iter = objectList.begin(); iter != objectList.end(); iter++ )
	{
		const Object* object = *iter;
		double distance;
		if( object->CalculateSurfacePoint( ray, surfacePoint, distance ) )
		{
			if( distance < smallestDistance || smallestDistance == -1.0 )
			{
				nearestSurfacePoint = surfacePoint;
				smallestDistance = distance;
			}
		}
	}

	if( smallestDistance != -1.0 )
	{
		surfacePoint = nearestSurfacePoint;
		return true;
	}

	return false;
}

//===========================================================================
// Return the absorbtion factors of light for (the color of) the given
// surface-point as seen from the given ray.
void Scene::CalculateSurfacePointColor( const Ray& ray, const SurfacePoint& surfacePoint, c3ga::vectorE3GA& color ) const
{
	color = surfacePoint.color;

	c3ga::vectorE3GA reflectionColor;
	if( surfacePoint.reflectionCoeficient > 0.0 )
	{
		Ray reflectionRay;
		surfacePoint.Reflect( ray, reflectionRay );
		CalculateVisibleColor( reflectionRay, reflectionColor );
	}
	color += c3ga::gp( reflectionColor, surfacePoint.reflectionCoeficient );

	c3ga::vectorE3GA refractionColor;
	if( surfacePoint.refractionCoeficient > 0.0 )
	{
		Ray refractionRay;
		surfacePoint.Refract( ray, refractionRay );
		CalculateVisibleColor( refractionRay, refractionColor );
	}
	color += c3ga::gp( refractionColor, surfacePoint.refractionCoeficient );
}

//===========================================================================
// Assuming nothing more than an ambient lighting model, ray-racing, as a
// visible surface determination algorithm, is as correct as we could hope
// it to be.  However, assming more than an ambient lighting model, ray-tracing,
// as an algorithm for determining not just what's visible, but also how it's
// visible, leaves much to be desired in that it it fails to account for indirect
// sources of light, unless perhaps highly sophisticated measures are taken.
// The light model we use here, therefore, will need some forgiveness.
void Scene::CalculateSurfacePointIllumination( const SurfacePoint& surfacePoint, c3ga::vectorE3GA& light ) const
{
	light.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 0.0 );
	for( LightList::const_iterator iter = lightList.begin(); iter != lightList.end(); iter++ )
	{
		const Light* lightSource = *iter;
		light += lightSource->CalculateSurfacePointIllumination( surfacePoint, objectList );
	}
}

//===========================================================================
Scene* Scene::Clone( void ) const
{
	return 0;
}

//===========================================================================
void Scene::SurfacePoint::Reflect( const Ray& ray, Ray& reflectionRay ) const
{
}

//===========================================================================
void Scene::SurfacePoint::Refract( const Ray& ray, Ray& refractionRay ) const
{
}

// Scene.cpp