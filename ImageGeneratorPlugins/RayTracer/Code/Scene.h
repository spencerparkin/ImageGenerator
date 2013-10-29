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
		c3ga::vectorE3GA point;
		c3ga::vectorE3GA direction;		// This should always be a unit-length vector.
	};

	//===========================================================================
	class SurfacePoint
	{
	public:

		void Reflect( const Ray& ray, Ray& reflectionRay ) const;
		void Refract( const Ray& ray, Ray& refractionRay ) const;

		c3ga::vectorE3GA point;
		c3ga::vectorE3GA normal;		// This should always be a unit-length vector.
		c3ga::vectorE3GA color;
		double reflectionCoeficient;
		double refractionCoeficient;
	};

	void CalculateVisibleColor( const Ray& ray, c3ga::vectorE3GA& visibleColor ) const;
	bool CalculateSurfacePoint( const Ray& ray, SurfacePoint& surfacePoint ) const;
	void CalculateSurfacePointColor( const Ray& ray, const SurfacePoint& surfacePoint, c3ga::vectorE3GA& color ) const;
	void CalculateSurfacePointIllumination( const SurfacePoint& surfacePoint, c3ga::vectorE3GA& light ) const;

	Scene* Clone( void ) const;

	//===========================================================================
	class Element
	{
	public:

		// Return a copy of this scene element.
		virtual Element* Clone( void ) const = 0;
	};

	//===========================================================================
	class Object : public Element
	{
	public:

		// Return the point on the surface of this object that is seen by the given ray, if any.
		// Also return the distance from the ray origin to the surface point.
		virtual bool CalculateSurfacePoint( const Ray& ray, SurfacePoint& surfacePoint, double& distance ) const = 0;
	};

	typedef std::list< Object* > ObjectList;

	//===========================================================================
	class Light : public Element
	{
	public:

		// Return the contribution of light made by this light source to the given surface point.
		// This is a non-trivial problem.  Our potential light contribution may be obstructed
		// by some object in the scene, or we may indirectly illuminate the given surface point.
		virtual c3ga::vectorE3GA CalculateSurfacePointIllumination( const SurfacePoint& surfacePoint, const ObjectList& objectList ) const = 0;
	};

	typedef std::list< Light* > LightList;

	void AddLight( Light* light );
	void AddObject( Object* object );

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
};

// Scene.h