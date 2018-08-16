// Tetrahedron.cpp

#include "Tetrahedron.h"
#include "Triangle.h"

Tetrahedron::Tetrahedron(void)
{
}

/*virtual*/ Tetrahedron::~Tetrahedron(void)
{
}

/*virtual*/ Scene::Element* Tetrahedron::Clone(void) const
{
	Tetrahedron* clone = new Tetrahedron();
	for( int i = 0; i < 4; i++ )
		clone->vertex[i] = vertex[i];
	return clone;
}

/*virtual*/ bool Tetrahedron::Configure(wxXmlNode* xmlNode)
{
	vertex[0] = Scene::LoadVector(xmlNode, "vertex0", vertex[0]);
	vertex[1] = Scene::LoadVector(xmlNode, "vertex1", vertex[1]);
	vertex[2] = Scene::LoadVector(xmlNode, "vertex2", vertex[2]);
	vertex[3] = Scene::LoadVector(xmlNode, "vertex3", vertex[3]);

	return true;
}

/*virtual*/ bool Tetrahedron::CalculateSurfacePoint(const Scene::Ray& ray, const Scene& scene, Scene::SurfacePoint& surfacePoint) const
{
	double minDistance = 999999999.0;
	bool hitSurface = false;

	for( int i = 0; i < 4; i++ )
	{
		Triangle triangle;
		c3ga::vectorE3GA* vertexPtr = triangle.vertex;
		for( int j = 0; j < 4; j++ )
			if( j != i )
				*vertexPtr++ = vertex[j];

		Scene::SurfacePoint triangleSurfacePoint;
		if( triangle.CalculateSurfacePoint( ray, scene, triangleSurfacePoint ) )
		{
			double distance = c3ga::norm( scene.Eye() - triangleSurfacePoint.point );
			if( distance < minDistance )
			{
				minDistance = distance;
				surfacePoint = triangleSurfacePoint;
				hitSurface = true;
			}
		}
	}

	return hitSurface;
}

double Tetrahedron::Volume( void ) const
{
	c3ga::vectorE3GA vector = c3ga::lc( vertex[3] - vertex[0], c3ga::op( vertex[1] - vertex[0], vertex[2] - vertex[0] ) );
	double volume = c3ga::norm( vector ) / 6.0;
	return volume;
}

// Tetrahedron.cpp