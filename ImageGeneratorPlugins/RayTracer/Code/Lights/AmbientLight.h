// AmbientLight.h

//===========================================================================
class AmbientLight : public Scene::Light
{
public:

	AmbientLight( const c3ga::vectorE3GA& intensity );
	virtual ~AmbientLight( void );

	virtual Scene::Element* Clone( void ) const override;
	virtual c3ga::vectorE3GA CalculateSurfacePointIllumination( const Scene::SurfacePoint& surfacePoint, const Scene::ObjectList& objectList ) const override;

private:

	c3ga::vectorE3GA intensity;
};

// AmbientLight.h