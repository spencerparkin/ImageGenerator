// SetOperation.cpp

#include "../Header.h"

//===========================================================================
SetOperation::SetOperation( void )
{
}

//===========================================================================
SetOperation::SetOperation( const Scene::MaterialProperties& materialProperties ) : Object( materialProperties )
{
	// We'll ignore the given material and use the surface point properties of the sub-objects we hit.
	// I'm not sure what to do with the material for now.
}

//===========================================================================
/*virtual*/ SetOperation::~SetOperation( void )
{
	while( objectList.size() > 0 )
	{
		ObjectList::iterator iter = objectList.begin();
		Object* object = *iter;
		delete object;
		objectList.erase( iter );
	}
}

//===========================================================================
/*virtual*/ SetOperation::Element* SetOperation::Clone( void ) const
{
	return nullptr;
}

//===========================================================================
/*virtual*/ bool SetOperation::Configure( wxXmlNode* xmlNode )
{
	if( !Object::Configure( xmlNode ) )
		return false;

	if( !materialProperties.Configure( Scene::FindNode( xmlNode, "material" ) ) )
		return false;

	wxString operationTypeStr;
	if( !xmlNode->GetAttribute( "operationType", &operationTypeStr ) )
		return false;

	if( operationTypeStr == "union" )
		operationType = OPERATION_UNION;
	else if( operationTypeStr == "intersection" )
		operationType = OPERATION_INTERSECTION;
	else if( operationTypeStr == "difference" )
		operationType = OPERATION_DIFFERENCE;
	else
		return false;

	wxXmlNode* xmlSubObjectsNode = Scene::FindNode( xmlNode, "subObjects" );
	if( !xmlSubObjectsNode )
		return false;

	wxXmlNode* xmlChildNode = xmlSubObjectsNode;
	while( xmlChildNode )
	{
		Scene::Element* element = RayTracerPlugin::CreateElement( xmlChildNode );
		if( !element || !dynamic_cast< Scene::Object* >( element ) )
		{
			delete element;
			return false;
		}

		objectList.push_back( ( Scene::Object* )element );

		xmlChildNode = xmlChildNode->GetNext();
	}

	return true;
}

//===========================================================================
/*virtual*/ bool SetOperation::CalculateSurfacePoint( const Scene::Ray& ray, const Scene& scene, Scene::SurfacePoint& surfacePoint ) const
{
	// TODO: Hmmm...we would need to support more than just CalculateSurfacePoint on each kind of object.
	//       Maybe require each object to impliment a routine that returns a list of surface points, and we can know
	//       that it goes in, out, in, out, etc., along that list?

	// Step 1: Generate the in/out surface-point list for each sub-object.
	// Step 2: Based upon our set operation, calculate the appropriate surface point.

	return false;
}

//===========================================================================
/*virtual*/ Scene::Object::Side Sphere::CalculateRaySide( const Scene::Ray& ray ) const
{
	// Hmmm...not sure what to do here.

	return OUTSIDE;
}

//===========================================================================
/*virtual*/ bool Sphere::CalculateTextureCoordinates( const c3ga::vectorE3GA& point, c3ga::vectorE3GA& textureCoordinates ) const
{
	return false;
}

// SetOperation.cpp