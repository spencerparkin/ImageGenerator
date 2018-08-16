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
	clone->tetrahedron = tetrahedron;
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

	if( !tetrahedron.Configure( Scene::FindNode( xmlNode, "tetrahedron" ) ) )
		return false;

	maxIterations = ( int )Scene::LoadNumber( xmlNode, "maxIterations", maxIterations );

	return true;
}

//===========================================================================
/*virtual*/ void SierpinskiTetrahedron::PrepareForDistanceEstimate(void) const
{
	tetrahedronList.clear();
}

//===========================================================================
/*virtual*/ bool SierpinskiTetrahedron::CalculateSurfacePoint( const Scene::Ray& ray, const Scene& scene, Scene::SurfacePoint& surfacePoint ) const
{
	if( !RayMarch( ray, surfacePoint.point, 64, 0.01 ) )
		return false;

	/*bool foundNormal = false;
	for( std::list< Tetrahedron >::reverse_iterator iter = tetrahedronList.rbegin(); iter != tetrahedronList.rend(); iter++ )
	{
		const Tetrahedron& normalTetrahedron = *iter;
		Scene::SurfacePoint normalSurfacePoint;
		if( normalTetrahedron.CalculateSurfacePoint( ray, scene, normalSurfacePoint) )
		{
			surfacePoint.normal = normalSurfacePoint.normal;
			foundNormal = true;
			break;
		}
	}

	if( foundNormal == false )
	{
		surfacePoint.normal.set(c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 0.0);
	}*/

	std::list< Tetrahedron >::iterator iter = tetrahedronList.begin();
	iter++;
	iter++;
	iter++;
	const Tetrahedron& normalTetrahedron = *iter;
	Scene::SurfacePoint normalSurfacePoint;
	if( normalTetrahedron.CalculateSurfacePoint( ray, scene, normalSurfacePoint ) )
	{
		surfacePoint.normal = normalSurfacePoint.normal;
	}
	else
	{
		surfacePoint.normal.set(c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 0.0);
	}

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

		tetrahedronList.push_back( subTetrahedron );

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