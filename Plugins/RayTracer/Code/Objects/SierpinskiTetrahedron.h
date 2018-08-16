// SierpinskiTetrahedron.h

#pragma once

#include "Fractal.h"

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
	struct Tetrahedron
	{
		c3ga::vectorE3GA vertex[4];
	};
	Tetrahedron tetrahedron;
	int maxIterations;
};

// SierpinskiTetrahedron.h