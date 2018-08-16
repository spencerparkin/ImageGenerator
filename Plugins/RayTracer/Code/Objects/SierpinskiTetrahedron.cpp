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
	
	double delta = 0.01;

	c3ga::vectorE3GA xVec(c3ga::vectorE3GA::coord_e1_e2_e3, delta, 0.0, 0.0);
	c3ga::vectorE3GA yVec(c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, delta, 0.0);
	c3ga::vectorE3GA zVec(c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, delta);

	// I don't have any intuitive idea as to why this would work.  I got this from the internet.
	surfacePoint.normal.m_e1 = DistanceEstimate( surfacePoint.point + xVec ) - DistanceEstimate( surfacePoint.point - xVec );
	surfacePoint.normal.m_e2 = DistanceEstimate( surfacePoint.point + yVec ) - DistanceEstimate( surfacePoint.point - yVec );
	surfacePoint.normal.m_e3 = DistanceEstimate( surfacePoint.point + zVec ) - DistanceEstimate( surfacePoint.point - zVec );

	surfacePoint.normal = c3ga::unit( surfacePoint.normal );

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