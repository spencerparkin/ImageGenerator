// SetOperation.h

#pragma once

//===========================================================================
class SetOperation : public Scene::Object
{
public:
	SetOperation( void );
	SetOperation( const Scene::MaterialProperties& materialProperties );
	virtual ~SetOperation( void );

	virtual Scene::Element* Clone( void ) const override;
	virtual bool Configure( wxXmlNode* xmlNode ) override;
	virtual bool CalculateSurfacePoint( const Scene::Ray& ray, const Scene& scene, Scene::SurfacePoint& surfacePoint ) const override;
	virtual Side CalculateRaySide( const Scene::Ray& ray ) const override;
	virtual bool CalculateTextureCoordinates( const c3ga::vectorE3GA& point, c3ga::vectorE3GA& textureCoordinates ) const override;

	enum OperationType
	{
		OPERATION_UNION,
		OPERATION_INTERSECTION,
		OPERATION_DIFFERENCE,
	};

	OperationType operationType;
	Scene::ObjectList objectList;
};

// SetOperation.h