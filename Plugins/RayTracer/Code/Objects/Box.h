// Box.h

#pragma once

#include "../Scene.h"

//===========================================================================
class Box : public Scene::Object
{
public:
	Box( void );
	virtual ~Box( void );

	virtual Scene::Element* Clone( void ) const override;
	virtual bool Configure( wxXmlNode* xmlNode ) override;
	virtual bool CalculateSurfacePoint( const Scene::Ray& ray, const Scene& scene, Scene::SurfacePoint& surfacePoint ) const override;
	virtual Side CalculateRaySide( const Scene::Ray& ray ) const override;
	virtual bool CalculateTextureCoordinates( const c3ga::vectorE3GA& point, c3ga::vectorE3GA& textureCoordinates ) const override;

private:
	
	c3ga::vectorE3GA center;
	c3ga::vectorE3GA dimensions;
	c3ga::rotorE3GA orientation;		// Put ray in object space to do ray-box intersection test.
};

// Box.h