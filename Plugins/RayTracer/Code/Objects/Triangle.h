// Triangle.h

#pragma once

#include "../Scene.h"

//===========================================================================
class Triangle : public Scene::Object
{
public:
	Triangle(void);
	virtual ~Triangle(void);

	virtual Scene::Element* Clone(void) const override;
	virtual bool Configure(wxXmlNode* xmlNode) override;
	virtual bool CalculateSurfacePoint(const Scene::Ray& ray, const Scene& scene, Scene::SurfacePoint& surfacePoint) const override;

	double Area( void ) const;

	c3ga::vectorE3GA vertex[3];
};

// Triangle.h