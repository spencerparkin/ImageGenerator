// AmbientLight.cpp

#include "../Header.h"

//===========================================================================
AmbientLight::AmbientLight( void )
{
	intensity.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 0.0 );
}

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
/*virtual*/ bool AmbientLight::Configure( wxXmlNode* xmlNode )
{
	intensity = Scene::LoadColor( xmlNode, "intensity", intensity );
	return true;
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