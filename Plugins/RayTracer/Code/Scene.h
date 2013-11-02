// Scene.h

//===========================================================================
class Scene
{
	friend class CallStackCounter;

public:

	Scene( void );
	~Scene( void );

	//===========================================================================
	struct Ray
	{
		c3ga::vectorE3GA CalculateRayPoint( double lambda ) const;

		// This is the point at which the ray originates.
		c3ga::vectorE3GA point;

		// Always being a unit-length vector, the ray parameter (lambda) may
		// also be thought of as distance along the ray.
		c3ga::vectorE3GA direction;
	};

	//===========================================================================
	// Here we characterize a material by the percentages of various types of
	// light intensities that are absorbed/reflected by the material's surface.
	class MaterialProperties
	{
	public:

		MaterialProperties( void );

		bool Configure( wxXmlNode* xmlNode );

		c3ga::vectorE3GA ambientLightCoeficient;
		c3ga::vectorE3GA diffuseReflectionCoeficient;
		c3ga::vectorE3GA specularReflectionCoeficient;
		c3ga::vectorE3GA reflectedLightCoeficient;
		c3ga::vectorE3GA refractedLightCoeficient;
		double specularReflectionExponent;
	};

	//===========================================================================
	// Here we accumulate various types of light intensities as they are
	// collected from various light sources.
	class LightSourceIntensities
	{
	public:

		LightSourceIntensities( void );

		c3ga::vectorE3GA ambientLightIntensity;
		c3ga::vectorE3GA diffuseLightIntensity;
		c3ga::vectorE3GA specularLightIntensity;
		c3ga::vectorE3GA reflectedLightIntensity;
		c3ga::vectorE3GA refractedLightIntensity;
	};

	//===========================================================================
	class SurfacePoint
	{
	public:

		void Reflect( const Ray& ray, Ray& reflectedRay, double nudge ) const;
		void Refract( const Ray& ray, Ray& refractedRay, double nudge ) const;

		c3ga::vectorE3GA point;
		c3ga::vectorE3GA normal;		// This should always be a unit-length vector.
		MaterialProperties materialProperties;
	};

	void CalculateVisibleLight(
						const Ray& ray,
						c3ga::vectorE3GA& visibleLight ) const;
	bool CalculateVisibleSurfacePoint(
						const Ray& ray,
						SurfacePoint& surfacePoint ) const;
	void AccumulateSurfacePointLight(
						const Ray& ray,
						const SurfacePoint& surfacePoint,
						LightSourceIntensities& lightSourceIntensities ) const;
	void CalculateSurfacePointIllumination(
						const SurfacePoint& surfacePoint,
						LightSourceIntensities& lightSourceIntensities,
						c3ga::vectorE3GA& visibleLight ) const;

	Scene* Clone( void ) const;

	//===========================================================================
	class Element
	{
	public:

		Element( void ) {}
		virtual ~Element( void ) {}

		// Return a copy of this scene element.
		virtual Element* Clone( void ) const = 0;

		// Configure this scene element using the given XML node.
		virtual bool Configure( wxXmlNode* xmlNode ) = 0;
	};

	//===========================================================================
	class Object : public Element
	{
	public:

		Object( void ) {}
		Object( const MaterialProperties& materialProperties );
		virtual ~Object( void ) {}

		// Return the point on the surface of this object that is seen by the given ray, if any.
		// By definition, this must be a point that is on the given ray, which is not necessarily
		// a point on the line determined by the ray.  Specifically, the ray parameter for a ray
		// point is always a non-negative real number.
		virtual bool CalculateSurfacePoint( const Ray& ray, SurfacePoint& surfacePoint ) const = 0;

	protected:

		MaterialProperties materialProperties;
	};

	typedef std::list< Object* > ObjectList;

	//===========================================================================
	class Light : public Element
	{
	public:

		Light( void ) {}
		virtual ~Light( void ) {}

		// Accumulate the contribution of light made by this light source to the given surface point.
		// This is a non-trivial problem.  Our potential light contribution may be obstructed
		// by some object in the scene, or we may indirectly illuminate the given surface point.
		virtual void AccumulateSurfacePointLight(
							const SurfacePoint& surfacePoint,
							const Scene& scene,
							LightSourceIntensities& lightSourceIntensities ) const = 0;
	};

	typedef std::list< Light* > LightList;

	void AddLight( Light* light );
	void AddObject( Object* object );

	void Clear( void );

	static c3ga::vectorE3GA LoadColor( wxXmlNode* xmlNode, const wxString& nodeName, const c3ga::vectorE3GA& defaultColor );
	static c3ga::vectorE3GA LoadVector( wxXmlNode* xmlNode, const wxString& nodeName, const c3ga::vectorE3GA& defaultVector );
	static c3ga::vectorE3GA LoadTriplet( wxXmlNode* xmlNode, const wxString& nodeName, const c3ga::vectorE3GA& defaultTriplet, const char* components );
	static double LoadNumber( wxXmlNode* xmlNode, const wxString& nodeName, double defaultNumber );
	static wxXmlNode* FindNode( wxXmlNode* xmlNode, const wxString& nodeName );

	const c3ga::vectorE3GA& Eye( void ) const;
	void Eye( const c3ga::vectorE3GA& eye );

private:

	//===========================================================================
	class RayBounceDepthCounter
	{
	public:
		RayBounceDepthCounter( const Scene* scene );
		~RayBounceDepthCounter( void );
	private:
		const Scene* scene;
	};

	// This counter variable is one reason why we need each thread to use
	// its own clone of the scene class instance;
	mutable int rayBounceDepthCount;
	mutable int maxRayBounceDepthCount;

	LightList lightList;

	// To speed up ray-casts, we may at one point, if the number
	// of geometries in the scene necessitates it, decide to store
	// the geometries in some sort of spacial sorting data-structure.
	ObjectList objectList;

	c3ga::vectorE3GA eye;

	static c3ga::vectorE3GA ComponentWiseMultiply(
								const c3ga::vectorE3GA& vectorA,
								const c3ga::vectorE3GA& vectorB );
};

// Scene.h