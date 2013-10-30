// Sphere.h

//===========================================================================
class Sphere : public Scene::Object
{
public:
	Sphere( const c3ga::vectorE3GA& center, double radius, const Scene::MaterialProperties& materialProperties );
	Sphere( const c3ga::dualSphere& dualSphere, const Scene::MaterialProperties& materialProperties );
	virtual ~Sphere( void );

	virtual Scene::Element* Clone( void ) const override;
	virtual bool CalculateSurfacePoint( const Scene::Ray& ray, Scene::SurfacePoint& surfacePoint ) const override;

private:
	c3ga::dualSphere dualSphere;
};

// Sphere.h