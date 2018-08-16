// AmbientLight.h

#pragma once

#include "../Scene.h"

//===========================================================================
class AmbientLight : public Scene::Light
{
public:

	AmbientLight( void );
	AmbientLight( const c3ga::vectorE3GA& intensity );
	virtual ~AmbientLight( void );

	virtual Scene::Element* Clone( void ) const override;
	virtual bool Configure( wxXmlNode* xmlNode ) override;
	virtual void AccumulateSurfacePointLight(
						const Scene::SurfacePoint& surfacePoint,
						const Scene& scene,
						Scene::LightSourceIntensities& lightSourceIntensities ) const override;

private:

	c3ga::vectorE3GA intensity;
};

// AmbientLight.h