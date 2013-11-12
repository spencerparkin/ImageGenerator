// Scene.cpp

#include "Header.h"

//===========================================================================
Scene::Scene( void )
{
	rayBounceDepthCount = 0;
	maxRayBounceDepthCount = 5;

	spaceColor.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 0.0 );
}

//===========================================================================
Scene::~Scene( void )
{
	Clear();
}

//===========================================================================
const c3ga::vectorE3GA& Scene::Eye( void ) const
{
	return eye;
}

//===========================================================================
void Scene::Eye( const c3ga::vectorE3GA& eye )
{
	this->eye = eye;
}

//===========================================================================
const c3ga::vectorE3GA&  Scene::SpaceColor( void ) const
{
	return spaceColor;
}

//===========================================================================
void Scene::SpaceColor( const c3ga::vectorE3GA& spaceColor )
{
	this->spaceColor = spaceColor;
}

//===========================================================================
void Scene::SetMaxRayBounceDepthCount( int count )
{
	maxRayBounceDepthCount = count;
}

//===========================================================================
int Scene::GetMaxRayBounceDepthCount( void ) const
{
	return maxRayBounceDepthCount;
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
	else if( rayBounceDepthCount == 1 )
	{
		// This doesn't quite work in all cases.
		// A better solution may be to ray-trace something
		// inside of a box or sphere of one solid color.
		visibleLight = spaceColor;
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
	scene->eye = eye;
	scene->spaceColor = spaceColor;

	return scene;
}

//===========================================================================
void Scene::SurfacePoint::Reflect( const Ray& ray, Ray& reflectedRay, double nudge ) const
{
	reflectedRay.direction = normal * -ray.direction * normal;

	// The direction vector is already normalized, but renormalize it
	// again to account for accumulated round-off error.
	reflectedRay.direction = c3ga::unit( reflectedRay.direction );

	// We nudge the reflected ray originating point a bit, because we
	// don't want to see the surface we're originating from.
	reflectedRay.point = point;
	reflectedRay.point = reflectedRay.CalculateRayPoint( nudge );
}

//===========================================================================
void Scene::SurfacePoint::Refract( const Ray& ray, Ray& refractedRay, double nudge ) const
{
	Object::Side side = Object::OUTSIDE;
	if( object )
		side = object->CalculateRaySide( ray );

	double indexOfRefractionForSpace = 1.0;	// In the future, we might let this be configurable.
	double indexOfRefractionForOldMedium, indexOfRefractionForNewMedium;
	switch( side )
	{
		case Object::OUTSIDE:
		{
			indexOfRefractionForOldMedium = indexOfRefractionForSpace;
			indexOfRefractionForNewMedium = materialProperties.indexOfRefraction;
			break;
		}
		case Object::INSIDE:
		{
			indexOfRefractionForOldMedium = materialProperties.indexOfRefraction;
			indexOfRefractionForNewMedium = indexOfRefractionForSpace;
			break;
		}
	}

	double cosRayAngle = c3ga::lc( normal, -ray.direction );
	double sinRayAngle = sqrt( 1.0 - cosRayAngle * cosRayAngle );
	double refractionAngle = asin( sinRayAngle * ( indexOfRefractionForOldMedium / indexOfRefractionForNewMedium ) );
	c3ga::bivectorE3GA blade = c3ga::unit( c3ga::op( normal, -ray.direction ) );
	c3ga::rotorE3GA rotor = c3ga::exp( blade * -refractionAngle * 0.5 );
	refractedRay.direction = c3ga::applyUnitVersor( rotor, -normal );

	// The direction vector is already normalized, but again, here we
	// renormalize it to account for any accumulated round-off error.
	refractedRay.direction = c3ga::unit( refractedRay.direction );

	// Again, nudge the ray origin point so that we can see beyond
	// the surface from which it originates.
	refractedRay.point = point;
	refractedRay.point = refractedRay.CalculateRayPoint( nudge );
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
	specularReflectionExponent = 1.0;
	indexOfRefraction = 1.0;
}

//===========================================================================
bool Scene::MaterialProperties::Configure( wxXmlNode* xmlNode )
{
	if( !xmlNode )
		return false;

	ambientLightCoeficient = LoadColor( xmlNode, "ambient", ambientLightCoeficient );
	diffuseReflectionCoeficient = LoadColor( xmlNode, "diffuse", diffuseReflectionCoeficient );
	specularReflectionCoeficient = LoadColor( xmlNode, "specular", specularReflectionCoeficient );
	reflectedLightCoeficient = LoadColor( xmlNode, "reflective", reflectedLightCoeficient );
	refractedLightCoeficient = LoadColor( xmlNode, "refractive", refractedLightCoeficient );
	specularReflectionExponent = LoadNumber( xmlNode, "specularExp", specularReflectionExponent );
	indexOfRefraction = LoadNumber( xmlNode, "refractionIndex", indexOfRefraction );
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
Scene::SurfacePoint::SurfacePoint( void )
{
	object = 0;
}

//===========================================================================
Scene::Object::Object( const MaterialProperties& materialProperties )
{
	this->materialProperties = materialProperties;
}

//===========================================================================
/*virtual*/ Scene::Object::~Object( void )
{
	DeleteTextures();
}

//===========================================================================
/*virtual*/ bool Scene::Object::Configure( wxXmlNode* xmlNode )
{
	for( wxXmlNode* textureNode = xmlNode->GetChildren(); textureNode; textureNode = textureNode->GetNext() )
	{
		if( textureNode->GetName() == "texture" )
		{
			Texture* texture = new Texture();
			if( !texture->Configure( textureNode ) )
				delete texture;
			else
				textureList.push_back( texture );
		}
	}

	return true;
}

//===========================================================================
void Scene::Object::DeleteTextures( void )
{
	while( textureList.size() > 0 )
	{
		TextureList::iterator iter = textureList.begin();
		Texture* texture = *iter;
		delete texture;
		textureList.erase( iter );
	}
}

//===========================================================================
void Scene::Object::CloneTextures( const Object* object )
{
	for( TextureList::const_iterator iter = object->textureList.begin(); iter != object->textureList.end(); iter++ )
	{
		const Texture* texture = *iter;
		Texture* clone = texture->Clone();
		textureList.push_back( clone );
	}
}

//===========================================================================
void Scene::Object::ApplyTextures( SurfacePoint& surfacePoint ) const
{
	c3ga::vectorE3GA textureCoordinates;
	if( CalculateTextureCoordinates( surfacePoint.point, textureCoordinates ) )
	{
		for( TextureList::const_iterator iter = textureList.begin(); iter != textureList.end(); iter++ )
		{
			const Texture* texture = *iter;
			surfacePoint.ApplyTexture( texture, textureCoordinates );
		}
	}
}

//===========================================================================
/*virtual*/ bool Scene::Object::CalculateTextureCoordinates( const c3ga::vectorE3GA& point, c3ga::vectorE3GA& textureCoordinates ) const
{
	return false;
}

//===========================================================================
void Scene::SurfacePoint::ApplyTexture( const Texture* texture, const c3ga::vectorE3GA& textureCoordinates )
{
	c3ga::vectorE3GA textureData;
	if( texture->CalculateTextureData( textureCoordinates, textureData ) )
	{
		switch( texture->GetType() )
		{
			case Texture::AMBIENT_LIGHT:
			{
				materialProperties.ambientLightCoeficient = textureData;
				break;
			}
			case Texture::DIFFUSE_REFLECTION:
			{
				materialProperties.diffuseReflectionCoeficient = textureData;
				break;
			}
			case Texture::SPECULAR_REFLECTION:
			{
				materialProperties.specularReflectionCoeficient = textureData;
				break;
			}
			case Texture::BUMP_MAP:
			{
				normal = textureData;
				break;
			}
		}
	}
}

//===========================================================================
Scene::Texture::Texture( Type type /*= DIFFUSE_REFLECTION*/, Mode mode /*= CLAMP*/, Filter filter /*= BILINEAR*/ )
{
	this->type = type;
	this->mode = mode;
	this->filter = filter;
	image = 0;
}

//===========================================================================
/*virtual*/ Scene::Texture::~Texture( void )
{
	delete image;
}

//===========================================================================
void Scene::Texture::SetType( Type type )
{
	this->type = type;
}

//===========================================================================
Scene::Texture::Type Scene::Texture::GetType( void ) const
{
	return type;
}

//===========================================================================
bool Scene::Texture::Configure( wxXmlNode* xmlNode )
{
	wxString typeString;
	if( xmlNode->GetAttribute( "type", &typeString ) )
	{
		if( typeString == "ambient" )
			type = AMBIENT_LIGHT;
		else if( typeString == "diffuse" )
			type = DIFFUSE_REFLECTION;
		else if( typeString == "specular" )
			type = SPECULAR_REFLECTION;
		else if( typeString == "bump" )
			type = BUMP_MAP;
	}

	wxString modeString;
	if( xmlNode->GetAttribute( "mode", &modeString ) )
	{
		if( modeString == "wrap" )
			mode = WRAP;
		else if( modeString == "clamp" )
			mode = CLAMP;
	}

	wxString filterString;
	if( xmlNode->GetAttribute( "filter", &filterString ) )
	{
		if( filterString == "nearest" )
			filter = NEAREST;
		else if( filterString == "bilinear" )
			filter = BILINEAR;
	}

	wxString textureFile = xmlNode->GetNodeContent();
	if( textureFile.IsEmpty() )
		return false;

	image = new wxImage();
	if( !image->LoadFile( textureFile ) )
		return false;

	return true;
}

//===========================================================================
Scene::Texture* Scene::Texture::Clone( void ) const
{
	Texture* texture = new Texture( type, mode );
	if( image )
		texture->image = new wxImage( image->GetWidth(), image->GetHeight(), image->GetData(), 0, true );
	return texture;
}

//===========================================================================
bool Scene::Texture::CalculateTextureData( const c3ga::vectorE3GA& textureCoordinates, c3ga::vectorE3GA& textureData ) const
{
	if( !image )
		return false;

	// We might support 3D textures in the future.
	switch( type )
	{
		case AMBIENT_LIGHT:
		case DIFFUSE_REFLECTION:
		case SPECULAR_REFLECTION:
		case BUMP_MAP:
		{
			double u = textureCoordinates.get_e1();
			double v = textureCoordinates.get_e2();
			
			if( mode == CLAMP )
			{
				if( u < 0.0 ) u = 0.0;
				if( u > 1.0 ) u = 1.0;
				if( v < 0.0 ) v = 0.0;
				if( v > 1.0 ) v = 1.0;
			}
			else if( mode == WRAP )
			{
				u = fmod( u, 1.0 );
				v = fmod( v, 1.0 );
				if( u < 0.0 ) u += 1.0;
				if( v < 0.0 ) v += 1.0;
			}

			unsigned char r, g, b;
			CalculateTexel( u, v, r, g, b );

			textureData.set( c3ga::vectorE3GA::coord_e1_e2_e3,
									double( r ) / 255.0,
									double( g ) / 255.0,
									double( b ) / 255.0 );
			return true;
		}
	}

	return false;
}

//===========================================================================
bool Scene::Texture::CalculateTexel( double u, double v, unsigned char& r, unsigned char& g, unsigned char& b ) const
{
	if( u < 0.0 || u > 1.0 ) return false;
	if( v < 0.0 || v > 1.0 ) return false;

	switch( filter )
	{
		case NEAREST:
		{
			int col = int( u * double( image->GetWidth() - 1 ) );
			int row = int( v * double( image->GetHeight() - 1 ) );

			r = image->GetRed( col, row );
			g = image->GetGreen( col, row );
			b = image->GetBlue( col, row );

			return true;
		}
		case BILINEAR:
		{
			double col = u * double( image->GetWidth() - 1 );
			double row = v * double( image->GetHeight() - 1 );

			double col_floor = floor( col );
			double row_floor = floor( row );
			
			double col_lerp = col - col_floor;
			double row_lerp = row - row_floor;

			int col0 = int( col_floor );
			int col1 = col0 + 1;
			if( col1 >= image->GetWidth() )
				col1 = image->GetWidth() - 1;

			int row0 = int( row_floor );
			int row1 = row0 + 1;
			if( row1 >= image->GetHeight() )
				row1 = image->GetHeight() - 1;

			r = unsigned char( Lerp(
					Lerp( double( image->GetRed( col0, row0 ) ), double( image->GetRed( col1, row0 ) ), col_lerp ),
					Lerp( double( image->GetRed( col0, row1 ) ), double( image->GetRed( col1, row1 ) ), col_lerp ),
					row_lerp ) );
			g = unsigned char( Lerp(
					Lerp( double( image->GetGreen( col0, row0 ) ), double( image->GetGreen( col1, row0 ) ), col_lerp ),
					Lerp( double( image->GetGreen( col0, row1 ) ), double( image->GetGreen( col1, row1 ) ), col_lerp ),
					row_lerp ) );
			b = unsigned char( Lerp(
					Lerp( double( image->GetBlue( col0, row0 ) ), double( image->GetBlue( col1, row0 ) ), col_lerp ),
					Lerp( double( image->GetBlue( col0, row1 ) ), double( image->GetBlue( col1, row1 ) ), col_lerp ),
					row_lerp ) );

			return true;
		}
	}

	return false;
}

//===========================================================================
/*static*/ double Scene::Texture::Lerp( double v0, double v1, double t )
{
	return v0 + ( v1 - v0 ) * t;
}

//===========================================================================
c3ga::vectorE3GA Scene::Ray::CalculateRayPoint( double lambda ) const
{
	return point + c3ga::gp( direction, lambda );
}

// Scene.cpp