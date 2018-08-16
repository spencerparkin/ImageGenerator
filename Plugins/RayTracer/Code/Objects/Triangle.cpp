// Triangle.cpp

#include "Triangle.h"
#include "Plane.h"

Triangle::Triangle(void)
{
}

/*virtual*/ Triangle::~Triangle(void)
{
}

/*virtual*/ Scene::Element* Triangle::Clone(void) const
{
	Triangle* clone = new Triangle();
	for (int i = 0; i < 3; i++)
		clone->vertex[i] = vertex[i];
	return clone;
}

/*virtual*/ bool Triangle::Configure(wxXmlNode* xmlNode)
{
	vertex[0] = Scene::LoadVector(xmlNode, "vertex0", vertex[0]);
	vertex[1] = Scene::LoadVector(xmlNode, "vertex1", vertex[1]);
	vertex[2] = Scene::LoadVector(xmlNode, "vertex2", vertex[2]);

	return true;
}

/*virtual*/ bool Triangle::CalculateSurfacePoint(const Scene::Ray& ray, const Scene& scene, Scene::SurfacePoint& surfacePoint) const
{
	Plane plane;
	plane.center = vertex[0];
	plane.normal = c3ga::unit( c3ga::gp( c3ga::op( vertex[1] - vertex[0], vertex[2] - vertex[0] ), c3ga::I3 ) );

	if( !plane.CalculateSurfacePoint( ray, scene, surfacePoint ) )
		return false;

	double area = Area();
	double totalSubArea = 0.0;

	for( int i = 0; i < 3; i++ )
	{
		Triangle subTriangle;
		c3ga::vectorE3GA* vertexPtr = subTriangle.vertex;
		*vertexPtr++ = surfacePoint.point;
		for( int j = 0; j < 3; j++ )
			if( j != i )
				*vertexPtr++ = vertex[j];

		double subArea = subTriangle.Area();
		totalSubArea += subArea;
	}

	double eps = 1e-4;
	if( fabs( totalSubArea - area ) < eps )
		return true;

	return false;
}

double Triangle::Area( void ) const
{
	c3ga::bivectorE3GA bivector = c3ga::op( vertex[1] - vertex[0], vertex[2] - vertex[0] );
	double area = fabs( c3ga::norm( bivector ) / 2.0 );
	return area;
}

// Triangle.cpp