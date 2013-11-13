// Quadric.h

//===========================================================================
class Quadric : public Scene::Object
{
public:
	Quadric( void );
	virtual ~Quadric( void );

	virtual Scene::Element* Clone( void ) const override;
	virtual bool Configure( wxXmlNode* xmlNode ) override;
	virtual bool CalculateSurfacePoint( const Scene::Ray& ray, const Scene& scene, Scene::SurfacePoint& surfacePoint ) const override;

private:

	double EvaluateQuadratic( const c3ga::vectorE3GA& point, bool homogeneous = true ) const;
	c3ga::vectorE3GA EvaluateQuadraticGradient( const c3ga::vectorE3GA& point ) const;

	c3ga::vectorE3GA quadraticCoeficients;
	c3ga::vectorE3GA nonLinearCoeficients;
	c3ga::vectorE3GA linearCoeficients;
	double constantCoeficient;
};

// Quadric.h