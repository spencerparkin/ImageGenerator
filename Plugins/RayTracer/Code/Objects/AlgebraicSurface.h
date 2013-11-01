// AlgebraicSurface.h

//===========================================================================
class AlgebraicSurface : public Scene::Object
{
public:
	AlgebraicSurface( void );
	AlgebraicSurface( Polynomial* polynomial );
	virtual ~AlgebraicSurface( void );

	virtual Scene::Element* Clone( void ) const override;
	virtual bool Configure( wxXmlNode* xmlNode ) override;
	virtual bool CalculateSurfacePoint( const Scene::Ray& ray, Scene::SurfacePoint& surfacePoint ) const override;

	void SetPolynomial( Polynomial* polynomial );	// We take ownership of the given allocation, deleting it on destruction.
	Polynomial* GetPolynomial( void );
	const Polynomial* GetPolynomial( void ) const;

	bool FindRayHit( const Scene::Ray& ray, double& lambda ) const;

private:

	Polynomial* polynomial;
};

// AlgebraicSurface.h