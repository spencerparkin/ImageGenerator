// AmbientLight.cpp

#include "../Header.h"

//===========================================================================
AmbientLight::AmbientLight( const c3ga::vectorE3GA& intensity )
{
	this->intensity = intensity;
}

//===========================================================================
/*virtual*/ AmbientLight::~AmbientLight( void )
{
}

//===========================================================================
/*virtual*/ Scene::Element* AmbientLight::Clone( void ) const
{
	AmbientLight* ambientLight = new AmbientLight( intensity );
	return ambientLight;
}

//===========================================================================
/*virtual*/ void AmbientLight::AccumulateSurfacePointLight(
							const Scene::SurfacePoint& surfacePoint,
							const Scene& scene,
							Scene::LightSourceIntensities& lightSourceIntensities ) const
{
	// An ambient light illuminates all surfaces.
	// No object obstructs the light and we don't worry about indirect lighting.
	lightSourceIntensities.ambientLightIntensity += intensity;
}

// AmbientLight.cpp