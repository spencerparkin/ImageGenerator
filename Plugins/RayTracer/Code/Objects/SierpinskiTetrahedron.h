// SierpinskiTetrahedron.h

#pragma once

#include "Fractal.h"
#include "Tetrahedron.h"

//===========================================================================
class SierpinskiTetrahedron : public Fractal
{
public:
	SierpinskiTetrahedron( void );
	virtual ~SierpinskiTetrahedron( void );

	virtual Scene::Element* Clone( void ) const override;
	virtual bool Configure( wxXmlNode* xmlNode ) override;
	virtual bool CalculateSurfacePoint( const Scene::Ray& ray, const Scene& scene, Scene::SurfacePoint& surfacePoint ) const override;
	virtual double DistanceEstimate( const c3ga::vectorE3GA& point ) const override;

private:
	
	Tetrahedron tetrahedron;
	int maxDepth;
};

// SierpinskiTetrahedron.h