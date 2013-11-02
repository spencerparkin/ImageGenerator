// PointLight.cpp

#include "../Header.h"

//===========================================================================
PointLight::PointLight( void )
{
	intensity.set( c3ga::vectorE3GA::coord_e1_e2_e3, 1.0, 1.0, 1.0 );
	point.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 0.0 );
	shadowCastCoeficient = 1.0;

	attenuationCoeficients.constant = 1.0;
	attenuationCoeficients.linear = 0.0;
	attenuationCoeficients.quadratic = 0.0;
}

//===========================================================================
PointLight::PointLight( const c3ga::vectorE3GA& intensity, const c3ga::vectorE3GA& point, double shadowCastCoeficient /*= 1.0*/ )
{
	this->intensity = intensity;
	this->point = point;
	this->shadowCastCoeficient = shadowCastCoeficient;

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
	return new PointLight( intensity, point, shadowCastCoeficient );
}

//===========================================================================
/*virtual*/ bool PointLight::Configure( wxXmlNode* xmlNode )
{
	intensity = Scene::LoadColor( xmlNode, "intensity", intensity );
	point = Scene::LoadVector( xmlNode, "point", point );
	shadowCastCoeficient = Scene::LoadNumber( xmlNode, "shadow", shadowCastCoeficient );
	return true;
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

	bool inShadow = false;
	if( shadowCastCoeficient < 1.0 )
	{
		Scene::Ray surfacePointToLightRay;
		surfacePointToLightRay.point = surfacePoint.point;
		surfacePointToLightRay.direction = surfacePointToLightDirection;
		surfacePointToLightRay.point = surfacePointToLightRay.CalculateRayPoint( 1e-5 );
		Scene::SurfacePoint obstructingSurfacePoint;
		if( scene.CalculateVisibleSurfacePoint( surfacePointToLightRay, obstructingSurfacePoint ) )
		{
			double distanceToObstruction = c3ga::norm( surfacePoint.point - obstructingSurfacePoint.point );
			if( distanceToLight > distanceToObstruction )
			{
				// In this case there is an obstruction keeping light from reaching the surface point.
				// If we were to get fancy here, we would try to see if the obstruction was transparent,
				// etc., but that sounds way too complicated.  Instead, we'll simply consider the
				// surface point to be in shadow by not accumulating as much intensity.
				inShadow = true;
			}
		}
	}

	lightSourceIntensities.diffuseLightIntensity +=
						c3ga::gp( intensity, attenuationFactor * dotProduct *
						( inShadow ? shadowCastCoeficient : 1.0 ) );
}

// PointLight.cpp