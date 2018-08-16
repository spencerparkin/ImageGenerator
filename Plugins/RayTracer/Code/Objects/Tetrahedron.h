// Tetrahedron.h

#pragma once

#include "../Scene.h"

//===========================================================================
class Tetrahedron : public Scene::Object
{
public:
	Tetrahedron(void);
	virtual ~Tetrahedron(void);

	virtual Scene::Element* Clone(void) const override;
	virtual bool Configure(wxXmlNode* xmlNode) override;
	virtual bool CalculateSurfacePoint(const Scene::Ray& ray, const Scene& scene, Scene::SurfacePoint& surfacePoint) const override;

	double Volume( void ) const;

	c3ga::vectorE3GA vertex[4];
};

// Tetrahedron.h