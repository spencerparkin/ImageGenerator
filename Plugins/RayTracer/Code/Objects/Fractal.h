// Fractal.h

#pragma once

#include "../Scene.h"

//===========================================================================
class Fractal : public Scene::Object
{
public:
	Fractal( void );
	virtual ~Fractal( void );

	// Calculate a lower-bound on the minimum distance from the given point to the fractal.
	virtual double DistanceEstimate( const c3ga::vectorE3GA& point ) const = 0;

	bool RayMarch( const Scene::Ray& ray, c3ga::vectorE3GA& contactPoint, int maxIteration, double eps = 1e-7 ) const;
};

// Fractal.h