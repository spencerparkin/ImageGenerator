// PointLight.h

//===========================================================================
class PointLight : public Scene::Light
{
public:

	PointLight( const c3ga::vectorE3GA& intensity, const c3ga::vectorE3GA& point );
	virtual ~PointLight( void );

	virtual Scene::Element* Clone( void ) const override;
	virtual void AccumulateSurfacePointLight(
						const Scene::SurfacePoint& surfacePoint,
						const Scene& scene,
						Scene::LightSourceIntensities& lightSourceIntensities ) const override;

	struct AttenuationCoeficients
	{
		double constant;
		double linear;
		double quadratic;
	};

	AttenuationCoeficients attenuationCoeficients;

private:

	c3ga::vectorE3GA intensity;
	c3ga::vectorE3GA point;
};

// PointLight.h