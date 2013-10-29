// Sphere.h

//===========================================================================
class Sphere : public Scene::Object
{
public:
	Sphere( const c3ga::vectorE3GA& center, double radius, const c3ga::vectorE3GA& color );
	Sphere( const c3ga::dualSphere& dualSphere, const c3ga::vectorE3GA& color );
	virtual ~Sphere( void );

	virtual Scene::Element* Clone( void ) const override;
	virtual bool CalculateSurfacePoint( const Scene::Ray& ray, Scene::SurfacePoint& surfacePoint, double& distance ) const override;

private:
	c3ga::dualSphere dualSphere;
	c3ga::vectorE3GA color;
};

// Sphere.h