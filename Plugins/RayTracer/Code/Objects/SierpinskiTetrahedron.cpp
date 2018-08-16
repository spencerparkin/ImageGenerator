// SierpinskiTetrahedron.cpp

#include "SierpinskiTetrahedron.h"

//===========================================================================
SierpinskiTetrahedron::SierpinskiTetrahedron( void )
{
	maxIterations = 100;
}

//===========================================================================
/*virtual*/ SierpinskiTetrahedron::~SierpinskiTetrahedron( void )
{
}

//===========================================================================
/*virtual*/ Scene::Element* SierpinskiTetrahedron::Clone( void ) const
{
	SierpinskiTetrahedron* clone = new SierpinskiTetrahedron();
	for( int i = 0; i < 4; i++ )
		clone->tetrahedron.vertex[i] = tetrahedron.vertex[i];
	clone->maxIterations = maxIterations;
	return clone;
}

//===========================================================================
/*virtual*/ bool SierpinskiTetrahedron::Configure( wxXmlNode* xmlNode )
{
	if( !Object::Configure( xmlNode ) )
		return false;

	if( !materialProperties.Configure( Scene::FindNode( xmlNode, "material" ) ) )
		return false;

	tetrahedron.vertex[0] = Scene::LoadVector( xmlNode, "vertex0", tetrahedron.vertex[0] );
	tetrahedron.vertex[1] = Scene::LoadVector( xmlNode, "vertex1", tetrahedron.vertex[1] );
	tetrahedron.vertex[2] = Scene::LoadVector( xmlNode, "vertex2", tetrahedron.vertex[2] );
	tetrahedron.vertex[3] = Scene::LoadVector( xmlNode, "vertex3", tetrahedron.vertex[3] );

	maxIterations = ( int )Scene::LoadNumber( xmlNode, "maxIterations", maxIterations );

	return true;
}

//===========================================================================
/*virtual*/ bool SierpinskiTetrahedron::CalculateSurfacePoint( const Scene::Ray& ray, const Scene& scene, Scene::SurfacePoint& surfacePoint ) const
{
	if( !RayMarch( ray, surfacePoint.point, 64, 0.01 ) )
		return false;
	
	// This is dumb, but do it for now...
	surfacePoint.normal = -ray.direction;

	surfacePoint.materialProperties = this->materialProperties;
	surfacePoint.object = this;

	return true;
}

//===========================================================================
/*virtual*/ double SierpinskiTetrahedron::DistanceEstimate( const c3ga::vectorE3GA& point ) const
{
	double distanceEstimate = 0.0;
	Tetrahedron subTetrahedron = tetrahedron;
	int i = 0;

	while( true )
	{
		double minDistance = 9999999.0;
		int k = -1;

		for( int j = 0; j < 4; j++ )
		{
			double distance = c3ga::norm( subTetrahedron.vertex[j] - point );
			if( distance < minDistance )
			{
				minDistance = distance;
				k = j;
			}
		}

		distanceEstimate = minDistance;
		i++;
		if( i >= maxIterations )
			break;

		for( int j = 0; j < 4; j++ )
			if( j != k )
				subTetrahedron.vertex[j] = subTetrahedron.vertex[j] + 0.5 * ( subTetrahedron.vertex[k] - subTetrahedron.vertex[j] );
	}

	return distanceEstimate;
}

// SierpinskiTetrahedron.cpp