// Plane.h

//===========================================================================
class Plane : public Scene::Object
{
public:
	Plane( void );
	Plane(
		const c3ga::vectorE3GA& center,
		const c3ga::vectorE3GA& normal,
		const Scene::MaterialProperties& materialProperties );
	virtual ~Plane( void );

	virtual Scene::Element* Clone( void ) const override;
	virtual bool Configure( wxXmlNode* xmlNode ) override;
	virtual bool CalculateSurfacePoint( const Scene::Ray& ray, Scene::SurfacePoint& surfacePoint ) const override;

private:
	c3ga::vectorE3GA center;
	c3ga::vectorE3GA normal;
};

// Plane.h