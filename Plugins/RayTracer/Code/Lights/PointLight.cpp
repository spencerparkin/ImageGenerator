// PointLight.cpp

#include "../Header.h"

//===========================================================================
PointLight::PointLight( const c3ga::vectorE3GA& intensity, const c3ga::vectorE3GA& point )
{
	this->intensity = intensity;
	this->point = point;

	attenuationCoeficients.constant = 1.0;
	attenuationCoeficients.linear = 0.0;
	attenuationCoeficients.quadratic = 0.0;
}

//===========================================================================
/*virtual*/ PointLight::~PointLight( void )
{
}

//===========================================================================
/*virtual*/ Scene::Element* PointLight::Clone( void ) const
{
	return new PointLight( intensity, point );
}

//===========================================================================
/*virtual*/ void PointLight::AccumulateSurfacePointLight(
							const Scene::SurfacePoint& surfacePoint,
							const Scene& scene,
							Scene::LightSourceIntensities& lightSourceIntensities ) const
{
	c3ga::vectorE3GA surfacePointToLightDirection = point - surfacePoint.point;
	double distanceToLight = c3ga::norm( surfacePointToLightDirection );
	surfacePointToLightDirection = c3ga::gp( surfacePointToLightDirection, 1.0 / distanceToLight );

	double attenuationFactor = 1.0 / (
				attenuationCoeficients.constant +
				attenuationCoeficients.linear * distanceToLight +
				attenuationCoeficients.quadratic * distanceToLight * distanceToLight );

	if( attenuationFactor > 1.0 )
		attenuationFactor = 1.0;

	double dotProduct = c3ga::lc( surfacePoint.normal, surfacePointToLightDirection );
	if( dotProduct < 0.0 )
		return;

	// This bit of code prevents anything anywhere from being lit.  Why?
	/*Scene::Ray surfacePointToLightRay;
	surfacePointToLightRay.point = surfacePoint.point;
	surfacePointToLightRay.direction = surfacePointToLightDirection;
	Scene::SurfacePoint obstructingSurfacePoint;
	if( scene.CalculateVisibleSurfacePoint( surfacePointToLightRay, obstructingSurfacePoint ) )
	{
		// In this case there is an obstruction keeping light from reaching the surface point.
		// If we were to get fancy here, we would try to see if the obstruction was transparent,
		// etc., but that sounds way too complicated.  Instead, we'll simply consider the
		// surface point to be in shadow by not accumulating any intensity.
		return;
	}*/

	lightSourceIntensities.diffuseLightIntensity +=
					c3ga::gp( intensity, attenuationFactor * dotProduct );
}

// PointLight.cpp