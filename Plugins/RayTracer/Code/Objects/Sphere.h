// Sphere.h

//===========================================================================
class Sphere : public Scene::Object
{
public:
	Sphere( void );
	Sphere( const c3ga::vectorE3GA& center, double radius, const Scene::MaterialProperties& materialProperties );
	virtual ~Sphere( void );

	virtual Scene::Element* Clone( void ) const override;
	virtual bool Configure( wxXmlNode* xmlNode ) override;
	virtual bool CalculateSurfacePoint( const Scene::Ray& ray, Scene::SurfacePoint& surfacePoint, double minimumDistance ) const override;

private:
	c3ga::vectorE3GA center;
	double radius;
};

// Sphere.h