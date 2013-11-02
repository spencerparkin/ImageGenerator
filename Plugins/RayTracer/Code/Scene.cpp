// Scene.cpp

#include "Header.h"

//===========================================================================
Scene::Scene( void )
{
	rayBounceDepthCount = 0;
	maxRayBounceDepthCount = 5;
}

//===========================================================================
Scene::~Scene( void )
{
	Clear();
}

//===========================================================================
void Scene::AddLight( Light* light )
{
	lightList.push_back( light );
}

//===========================================================================
void Scene::AddObject( Object* object )
{
	objectList.push_back( object );
}

//===========================================================================
void Scene::Clear( void )
{
	while( lightList.size() > 0 )
	{
		LightList::iterator iter = lightList.begin();
		Light* light = *iter;
		delete light;
		lightList.erase( iter );
	}

	while( objectList.size() > 0 )
	{
		ObjectList::iterator iter = objectList.begin();
		Object* object = *iter;
		delete object;
		objectList.erase( iter );
	}
}

//===========================================================================
Scene::RayBounceDepthCounter::RayBounceDepthCounter( const Scene* scene )
{
	this->scene = scene;
	scene->rayBounceDepthCount++;
}

//===========================================================================
Scene::RayBounceDepthCounter::~RayBounceDepthCounter( void )
{
	scene->rayBounceDepthCount--;
}

//===========================================================================
// Return the light that is visible in the scene at the given point and
// looking in the given direction.
void Scene::CalculateVisibleLight( const Ray& ray, c3ga::vectorE3GA& visibleLight ) const
{
	visibleLight.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 0.0 );

	RayBounceDepthCounter rayBounceDepthCounter( this );
	if( rayBounceDepthCount > maxRayBounceDepthCount )
		return;

	// Determine the surface point, if any, that can be seen by the given ray and its material properties.
	SurfacePoint surfacePoint;
	if( CalculateVisibleSurfacePoint( ray, surfacePoint ) )
	{
		// Determine the various light intensities that are reaching this surface point.
		LightSourceIntensities lightSourceIntensities;
		AccumulateSurfacePointLight( ray, surfacePoint, lightSourceIntensities );

		// Now calculate the light reflected from the surface point
		// by the light gathered from various light sources.
		CalculateSurfacePointIllumination( surfacePoint, lightSourceIntensities, visibleLight );
	}
}

//===========================================================================
// Return the surface characteristics of the point that can be seen by the
// given ray.  If no surface point is seen from the given ray, false is returned.
bool Scene::CalculateVisibleSurfacePoint( const Ray& ray, SurfacePoint& surfacePoint ) const
{
	SurfacePoint nearestSurfacePoint;
	double smallestDistance = -1.0;

	// Perform a linear search.  If this becomes too slow, we may
	// need to use some sort of spacial sorting data-structure.
	for( ObjectList::const_iterator iter = objectList.begin(); iter != objectList.end(); iter++ )
	{
		const Object* object = *iter;
		double distance;
		if( object->CalculateSurfacePoint( ray, surfacePoint ) )
		{
			distance = c3ga::norm( surfacePoint.point - ray.point );
			if( distance < smallestDistance || smallestDistance == -1.0 )
			{
				nearestSurfacePoint = surfacePoint;
				smallestDistance = distance;
			}
		}
	}

	if( smallestDistance != -1.0 )
	{
		surfacePoint = nearestSurfacePoint;
		return true;
	}

	return false;
}

//===========================================================================
void Scene::AccumulateSurfacePointLight( const Ray& ray, const SurfacePoint& surfacePoint, LightSourceIntensities& lightSourceIntensities ) const
{
	// If the material reflects visible light, determine how much reflected light it is receiving.
	if( c3ga::norm2( surfacePoint.materialProperties.reflectedLightCoeficient ) > 0.0 )
	{
		Ray reflectedRay;
		surfacePoint.Reflect( ray, reflectedRay, 1e-5 );
		c3ga::vectorE3GA reflectedLight;
		CalculateVisibleLight( reflectedRay, reflectedLight );
		lightSourceIntensities.reflectedLightIntensity += reflectedLight;
	}

	// If the material refracts visible light, determine how much refracted light it is receiving.
	if( c3ga::norm2( surfacePoint.materialProperties.refractedLightCoeficient ) > 0.0 )
	{
		Ray refractedRay;
		surfacePoint.Refract( ray, refractedRay, 1e-5 );
		c3ga::vectorE3GA refractedLight;
		CalculateVisibleLight( refractedRay, refractedLight );
		lightSourceIntensities.refractedLightIntensity += refractedLight;
	}
	
	// Go collect light from all of the different potential light sources in the scene.
	// This is a non-trivial problem in the case of indirect lighting.  Some light sources
	// may need to take into account possible obstructions for the purpose of shadow casting.
	for( LightList::const_iterator iter = lightList.begin(); iter != lightList.end(); iter++ )
	{
		const Light* lightSource = *iter;
		lightSource->AccumulateSurfacePointLight( surfacePoint, *this, lightSourceIntensities );
	}
}

//===========================================================================
void Scene::CalculateSurfacePointIllumination(
							const SurfacePoint& surfacePoint,
							LightSourceIntensities& lightSourceIntensities,
							c3ga::vectorE3GA& visibleLight ) const
{
	visibleLight =
		ComponentWiseMultiply( lightSourceIntensities.ambientLightIntensity, surfacePoint.materialProperties.ambientLightCoeficient ) +
		ComponentWiseMultiply( lightSourceIntensities.diffuseLightIntensity, surfacePoint.materialProperties.diffuseReflectionCoeficient ) +
		ComponentWiseMultiply( lightSourceIntensities.specularLightIntensity, surfacePoint.materialProperties.specularReflectionCoeficient ) +
		ComponentWiseMultiply( lightSourceIntensities.reflectedLightIntensity, surfacePoint.materialProperties.reflectedLightCoeficient ) +
		ComponentWiseMultiply( lightSourceIntensities.refractedLightIntensity, surfacePoint.materialProperties.refractedLightCoeficient );

	if( visibleLight.get_e1() > 1.0 )
		visibleLight.set_e1( 1.0 );
	if( visibleLight.get_e2() > 1.0 )
		visibleLight.set_e2( 1.0 );
	if( visibleLight.get_e3() > 1.0 )
		visibleLight.set_e3( 1.0 );
}

//===========================================================================
/*static*/ c3ga::vectorE3GA Scene::ComponentWiseMultiply(
								const c3ga::vectorE3GA& vectorA,
								const c3ga::vectorE3GA& vectorB )
{
	c3ga::vectorE3GA product( c3ga::vectorE3GA::coord_e1_e2_e3,
					vectorA.get_e1() * vectorB.get_e1(),
					vectorA.get_e2() * vectorB.get_e2(),
					vectorA.get_e3() * vectorB.get_e3() );
	return product;
}

//===========================================================================
Scene* Scene::Clone( void ) const
{
	Scene* scene = new Scene();

	for( LightList::const_iterator iter = lightList.begin(); iter != lightList.end(); iter++ )
		scene->lightList.push_back( ( Light* )( *iter )->Clone() );

	for( ObjectList::const_iterator iter = objectList.begin(); iter != objectList.end(); iter++ )
		scene->objectList.push_back( ( Object* )( *iter )->Clone() );

	scene->maxRayBounceDepthCount = maxRayBounceDepthCount;

	return scene;
}

//===========================================================================
void Scene::SurfacePoint::Reflect( const Ray& ray, Ray& reflectedRay, double nudge ) const
{
	reflectedRay.point = point;
	reflectedRay.direction = normal * -ray.direction * normal;

	// The direction vector is already normalized, but renormalize it
	// again to account for accumulated round-off error.
	reflectedRay.direction = c3ga::unit( reflectedRay.direction );

	// We nudge the reflected ray originating point a bit, because we
	// don't want to see the surface we're originating from.
	reflectedRay.point = reflectedRay.CalculateRayPoint( nudge );
}

//===========================================================================
void Scene::SurfacePoint::Refract( const Ray& ray, Ray& refractedRay, double nudge ) const
{
}

//===========================================================================
/*static*/ c3ga::vectorE3GA Scene::LoadColor(
							wxXmlNode* xmlNode,
							const wxString& nodeName,
							const c3ga::vectorE3GA& defaultColor )
{
	const char* components = "rgb";
	return LoadTriplet( xmlNode, nodeName, defaultColor, components );
}

//===========================================================================
/*static*/ c3ga::vectorE3GA Scene::LoadVector(
							wxXmlNode* xmlNode,
							const wxString& nodeName,
							const c3ga::vectorE3GA& defaultVector )
{
	const char* components = "xyz";
	return LoadTriplet( xmlNode, nodeName, defaultVector, components );
}

//===========================================================================
/*static*/ c3ga::vectorE3GA Scene::LoadTriplet( wxXmlNode* xmlNode, const wxString& nodeName, const c3ga::vectorE3GA& defaultTriplet, const char* components )
{
	c3ga::vectorE3GA triplet = defaultTriplet;
	xmlNode = FindNode( xmlNode, nodeName );
	if( xmlNode )
	{
		wxString string[3];
		if( xmlNode->GetAttribute( wxString( components[0] ), &string[0] ) &&
			xmlNode->GetAttribute( wxString( components[1] ), &string[1] ) &&
			xmlNode->GetAttribute( wxString( components[2] ), &string[2] ) )
		{
			double values[3];
			if( string[0].ToDouble( &values[0] ) &&
				string[1].ToDouble( &values[1] ) &&
				string[2].ToDouble( &values[2] ) )
			{
				triplet.set( c3ga::vectorE3GA::coord_e1_e2_e3, values[0], values[1], values[2] );
			}
		}
	}
	return triplet;
}

//===========================================================================
/*static*/ double Scene::LoadNumber( wxXmlNode* xmlNode, const wxString& nodeName, double defaultNumber )
{
	double number = defaultNumber;
	xmlNode = FindNode( xmlNode, nodeName );
	if( xmlNode )
	{
		wxString contents = xmlNode->GetNodeContent();
		contents.ToDouble( &number );
	}
	return number;
}

//===========================================================================
/*static*/ wxXmlNode* Scene::FindNode( wxXmlNode* xmlNode, const wxString& nodeName )
{
	wxXmlNode* foundNode = 0;
	
	if( xmlNode->GetName() == nodeName )
		foundNode = xmlNode;

	if( !foundNode && xmlNode->GetChildren() )
		foundNode = FindNode( xmlNode->GetChildren(), nodeName );
	
	if( !foundNode && xmlNode->GetNext() )
		foundNode = FindNode( xmlNode->GetNext(), nodeName );

	return foundNode;
}

//===========================================================================
Scene::MaterialProperties::MaterialProperties( void )
{
	ambientLightCoeficient.set( c3ga::vectorE3GA::coord_e1_e2_e3, 1.0, 1.0, 1.0 );
	diffuseReflectionCoeficient.set( c3ga::vectorE3GA::coord_e1_e2_e3, 1.0, 1.0, 1.0 );
	specularReflectionCoeficient.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 0.0 );
	reflectedLightCoeficient.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 0.0 );
	refractedLightCoeficient.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 0.0 );
}

//===========================================================================
bool Scene::MaterialProperties::Configure( wxXmlNode* xmlNode )
{
	if( !xmlNode )
		return false;

	c3ga::vectorE3GA zeroCoeficient( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 0.0 );

	ambientLightCoeficient = LoadColor( xmlNode, "ambient", zeroCoeficient );
	diffuseReflectionCoeficient = LoadColor( xmlNode, "diffuse", zeroCoeficient );
	specularReflectionCoeficient = LoadColor( xmlNode, "specular", zeroCoeficient );
	reflectedLightCoeficient = LoadColor( xmlNode, "reflective", zeroCoeficient );
	refractedLightCoeficient = LoadColor( xmlNode, "refractive", zeroCoeficient );
	return true;
}

//===========================================================================
Scene::LightSourceIntensities::LightSourceIntensities( void )
{
	ambientLightIntensity.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 0.0 );
	diffuseLightIntensity.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 0.0 );
	specularLightIntensity.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 0.0 );
	reflectedLightIntensity.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 0.0 );
	refractedLightIntensity.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 0.0 );
}

//===========================================================================
Scene::Object::Object( const MaterialProperties& materialProperties )
{
	this->materialProperties = materialProperties;
}

//===========================================================================
c3ga::vectorE3GA Scene::Ray::CalculateRayPoint( double lambda ) const
{
	return point + c3ga::gp( direction, lambda );
}

// Scene.cpp