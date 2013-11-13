// AlgebraicSurface.cpp

#include "../Header.h"

//===========================================================================
AlgebraicSurface::AlgebraicSurface( void )
{
	polynomial = 0;
}

//===========================================================================
AlgebraicSurface::AlgebraicSurface( Polynomial* polynomial )
{
	SetPolynomial( polynomial );
}

//===========================================================================
/*virtual*/ AlgebraicSurface::~AlgebraicSurface( void )
{
	delete polynomial;
}

//===========================================================================
/*virtual*/ Scene::Element* AlgebraicSurface::Clone( void ) const
{
	AlgebraicSurface* algebraicSurface = new AlgebraicSurface();
	algebraicSurface->materialProperties = materialProperties;
	algebraicSurface->polynomial = polynomial ? polynomial->Clone() : 0;
	return algebraicSurface;
}

//===========================================================================
/*virtual*/ bool AlgebraicSurface::Configure( wxXmlNode* xmlNode )
{
	if( !materialProperties.Configure( xmlNode ) )
		return false;
	if( polynomial )
		return polynomial->Configure( xmlNode );
	return false;
}

//===========================================================================
/*virtual*/ bool AlgebraicSurface::CalculateSurfacePoint( const Scene::Ray& ray, const Scene& scene, Scene::SurfacePoint& surfacePoint ) const
{
	// The composition of two continuous functions is continuous.
	class Function : public RootFinder::ContinuousFunction
	{
	public:
		virtual double Evaluate( double lambda ) override
		{
			// If we needed a derivative of this we could use the chain rule.
			return polynomial->Evaluate( ray->CalculateRayPoint( lambda ) );
		}
		const Polynomial* polynomial;
		const Scene::Ray* ray;
	};

	Function function;
	function.ray = &ray;
	function.polynomial = polynomial;
	RootFinder rootFinder( &function );
	double lambda;
	if( !rootFinder.FindFirstRoot( 0.0, 50.0, 0.05, lambda ) )
		return false;

	surfacePoint.point = ray.CalculateRayPoint( lambda );
	polynomial->EvaluateGradient( surfacePoint.point, surfacePoint.normal );
	surfacePoint.normal = c3ga::unit( surfacePoint.normal );
	if( c3ga::lc( surfacePoint.normal, ray.direction ) > 0.0 )
		surfacePoint.normal = -surfacePoint.normal;
	surfacePoint.materialProperties = materialProperties;
	return true;
}

//===========================================================================
void AlgebraicSurface::SetPolynomial( Polynomial* polynomial )
{
	this->polynomial = polynomial;
}

//===========================================================================
Polynomial* AlgebraicSurface::GetPolynomial( void )
{
	return polynomial;
}

//===========================================================================
const Polynomial* AlgebraicSurface::GetPolynomial( void ) const
{
	return polynomial;
}

// AlgebraicSurface.cpp