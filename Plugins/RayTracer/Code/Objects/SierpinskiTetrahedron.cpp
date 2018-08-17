// SierpinskiTetrahedron.cpp

#include "SierpinskiTetrahedron.h"

//===========================================================================
SierpinskiTetrahedron::SierpinskiTetrahedron( void )
{
	maxDepth = 10;
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
	clone->maxDepth = maxDepth;
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

	maxDepth = ( int )Scene::LoadNumber( xmlNode, "maxDepth", maxDepth );

	return true;
}

//===========================================================================
/*virtual*/ bool SierpinskiTetrahedron::CalculateSurfacePoint( const Scene::Ray& ray, const Scene& scene, Scene::SurfacePoint& surfacePoint ) const
{
	if( !tetrahedron.CalculateSurfacePoint( ray, scene, surfacePoint ) )
		return false;

	Tetrahedron subTetrahedron = tetrahedron;

	struct Hit
	{
		Tetrahedron tetrahedron;
		Scene::SurfacePoint surfacePoint;
	};

	for( int i = 0; i < maxDepth; i++ )
	{
		std::list< Hit > hitList;

		for( int j = 0; j < 4; j++ )
		{
			Hit hit;
			hit.tetrahedron = subTetrahedron;

			for( int k = 0; k < 4; k++ )
				if( k != j )
					hit.tetrahedron.vertex[k] = hit.tetrahedron.vertex[k] + 0.5 * ( hit.tetrahedron.vertex[j] - hit.tetrahedron.vertex[k] );

			if( hit.tetrahedron.CalculateSurfacePoint( ray, scene, hit.surfacePoint ) )
				hitList.push_back( hit );
		}

		if( hitList.size() == 0 )
			return false;

		double minDistance = 9999999.0;
		for( std::list< Hit >::iterator iter = hitList.begin(); iter != hitList.end(); iter++ )
		{
			const Hit& hit = *iter;
			double distance = c3ga::norm( scene.Eye() - hit.surfacePoint.point );
			if( distance < minDistance )
			{
				minDistance = distance;
				surfacePoint = hit.surfacePoint;
				subTetrahedron = hit.tetrahedron;
			}
		}
	}

	surfacePoint.materialProperties = this->materialProperties;
	surfacePoint.object = this;
	return true;
}

//===========================================================================
/*virtual*/ double SierpinskiTetrahedron::DistanceEstimate( const c3ga::vectorE3GA& point ) const
{
	int maxIterations = 100;
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