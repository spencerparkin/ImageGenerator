// RayTracer.cpp

#include "Header.h"

//===========================================================================
extern "C" __declspec( dllexport ) RayTracerPlugin* NewImageGeneratorPlugin( void )
{
	return new RayTracerPlugin();
}

//===========================================================================
extern "C" __declspec( dllexport ) void DeleteImageGeneratorPlugin( igPlugin* igPlugin )
{
	delete igPlugin;
}

//===========================================================================
RayTracerPlugin::RayTracerPlugin( void ) : menuEventHandler( this )
{
	scene = 0;
}

//===========================================================================
/*virtual*/ RayTracerPlugin::~RayTracerPlugin( void )
{
}

//===========================================================================
RayTracerPlugin::MenuEventHandler::MenuEventHandler( RayTracerPlugin* rayTracerPlugin )
{
	this->rayTracerPlugin = rayTracerPlugin;

	ID_LoadScene = -1;
	ID_UnloadScene = -1;
}

//===========================================================================
/*virtual*/ RayTracerPlugin::MenuEventHandler::~MenuEventHandler( void )
{
}

//===========================================================================
void RayTracerPlugin::MenuEventHandler::InsertMenu( wxMenuBar* menuBar )
{
	int maxID = -1;
	for( unsigned int i = 0; i < menuBar->GetMenuCount(); i++ )
	{
		const wxMenu* menu = menuBar->GetMenu( i );
		const wxMenuItemList& menuItemList = menu->GetMenuItems();
		for( wxMenuItemList::const_iterator iter = menuItemList.begin(); iter != menuItemList.end(); iter++ )
		{
			const wxMenuItem* menuItem = *iter;
			if( maxID < menuItem->GetId() )
				maxID = menuItem->GetId();
		}
	}

	ID_LoadScene = ++maxID;
	ID_UnloadScene = ++maxID;

	wxMenu* rayTracerMenu = new wxMenu();
	wxMenuItem* loadSceneMenuItem = new wxMenuItem( rayTracerMenu, ID_LoadScene, "Load Scene", "Load a scene to be ray-traced." );
	wxMenuItem* unloadSceneMenuItem = new wxMenuItem( rayTracerMenu, ID_UnloadScene, "Unload Scene", "Unload the currently loaded scene, if any." );
	rayTracerMenu->Append( loadSceneMenuItem );
	rayTracerMenu->Append( unloadSceneMenuItem );

	menuBar->Append( rayTracerMenu, "Ray Tracer" );

	menuBar->Bind( wxEVT_MENU, &MenuEventHandler::OnLoadScene, this, ID_LoadScene );
	menuBar->Bind( wxEVT_MENU, &MenuEventHandler::OnUnloadScene, this, ID_UnloadScene );
	menuBar->Bind( wxEVT_UPDATE_UI, &MenuEventHandler::OnUpdateMenuItemUI, this, ID_LoadScene );
	menuBar->Bind( wxEVT_UPDATE_UI, &MenuEventHandler::OnUpdateMenuItemUI, this, ID_UnloadScene );
}

//===========================================================================
void RayTracerPlugin::MenuEventHandler::RemoveMenu( wxMenuBar* menuBar )
{
	int menuPos = menuBar->FindMenu( "Ray Tracer" );
	if( menuPos == wxNOT_FOUND )
		return;

	wxMenu* rayTracerMenu = menuBar->Remove( menuPos );
	delete rayTracerMenu;
}

//===========================================================================
void RayTracerPlugin::MenuEventHandler::OnLoadScene( wxCommandEvent& event )
{
	if( rayTracerPlugin->scene )
		return;

	wxFileDialog fileDialog( 0, "Load Scene",
								wxEmptyString, wxEmptyString,
								"Scene File (*.xml)|*.xml",
								wxFD_OPEN | wxFD_FILE_MUST_EXIST );

	if( fileDialog.ShowModal() == wxID_CANCEL )
		return;

	wxString scenePath = fileDialog.GetPath();
	if( !rayTracerPlugin->LoadScene( scenePath ) )
		wxMessageBox( "Failed to load scene file \"" + scenePath + "\"." );
}

//===========================================================================
void RayTracerPlugin::MenuEventHandler::OnUnloadScene( wxCommandEvent& event )
{
	rayTracerPlugin->UnloadScene();
}

//===========================================================================
void RayTracerPlugin::MenuEventHandler::OnUpdateMenuItemUI( wxUpdateUIEvent& event )
{
	if( event.GetId() == ID_LoadScene )
		event.Enable( rayTracerPlugin->scene ? false : true );
	else if( event.GetId() == ID_UnloadScene )
		event.Enable( rayTracerPlugin->scene ? true : false );
}

//===========================================================================
bool RayTracerPlugin::LoadScene( const wxString& sceneFile )
{
	wxXmlDocument xmlDocument;
	if( !xmlDocument.Load( sceneFile ) )
		return false;

	wxXmlNode* xmlRoot = xmlDocument.GetRoot();
	if( !xmlRoot || xmlRoot->GetName() != "scene" )
		return false;

	scene = new Scene();
	LoadView(0);

	for( wxXmlNode* xmlNode = xmlRoot->GetChildren(); xmlNode; xmlNode = xmlNode->GetNext() )
	{
		if( xmlNode->GetName() == "view" )
			LoadView( xmlNode );
		else if( xmlNode->GetName() == "object" || xmlNode->GetName() == "light" )
			LoadElement( xmlNode );
	}

	return true;
}

//===========================================================================
bool RayTracerPlugin::UnloadScene( void )
{
	delete scene;
	scene = 0;
	return true;
}

//===========================================================================
bool RayTracerPlugin::LoadView( wxXmlNode* xmlNode )
{
	view.eye.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, 30.0 );
	view.up.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 1.0, 0.0 );
	view.direction = c3ga::unit( -view.eye );
	view.angle = M_PI / 5.0;
	view.focalLength = 10.0;

	if( xmlNode )
	{
		view.eye = Scene::LoadVector( xmlNode, "eye", view.eye );
		view.up = Scene::LoadVector( xmlNode, "up", view.up );
		view.direction = c3ga::unit( Scene::LoadVector( xmlNode, "dir", view.direction ) );
		view.angle = Scene::LoadNumber( xmlNode, "angle", view.angle * 180.0 / M_PI ) * M_PI / 180.0;
		view.focalLength = Scene::LoadNumber( xmlNode, "focalLength", view.focalLength );
	}

	return true;
}

//===========================================================================
bool RayTracerPlugin::LoadElement( wxXmlNode* xmlNode )
{
	bool success = false;
	Scene::Element* element = 0;

	do
	{
		wxString type;
		if( !xmlNode->GetAttribute( "type", &type ) )
			break;

		bool isObject = false;
		bool isLight = false;

		if( type == "sphere" )				{ element = new Sphere(); isObject = true; }
		else if( type == "plane" )			{ element = new Plane(); isObject = true; }
		else if( type == "quadric" )		{ element = new Quadric(); isObject = true; }
		else if( type == "cylindricalInv" )	{ element = new AlgebraicSurface( new CylindricalInversion() ); isObject = true; }
		else if( type == "ambientLight" )	{ element = new AmbientLight(); isLight = true; }
		else if( type == "pointLight" )		{ element = new PointLight(); isLight = true; }

		if( !element || !( isObject || isLight ) )
			break;

		if( !element->Configure( xmlNode ) )
			break;

		if( xmlNode->GetName() == "object" )
			scene->AddObject( ( Scene::Object* )element );
		else if( xmlNode->GetName() == "light" )
			scene->AddLight( ( Scene::Light* )element );
		else
			break;

		success = true;
	}
	while( false );

	if( !success )
		delete element;

	return success;
}

//===========================================================================
/*virtual*/ bool RayTracerPlugin::Initialize( wxMenuBar* menuBar )
{
	menuEventHandler.InsertMenu( menuBar );
	return true;
}

//===========================================================================
/*virtual*/ bool RayTracerPlugin::Finalize( wxMenuBar* menuBar )
{
	UnloadScene();

	if( menuBar )
		menuEventHandler.RemoveMenu( menuBar );

	return true;
}

//===========================================================================
/*virtual*/ wxString RayTracerPlugin::Name( void )
{
	return "Ray Tracer";
}

//===========================================================================
/*virtual*/ bool RayTracerPlugin::PreImageGeneration( wxImage* image )
{
	if( !scene )
		return false;

	return true;
}

//===========================================================================
/*virtual*/ bool RayTracerPlugin::PostImageGeneration( wxImage* image )
{
	return true;
}

//===========================================================================
RayTracerPlugin::ImageGenerator::ImageGenerator( void )
{
	scene = 0;
	plugin = 0;
}

//===========================================================================
/*virtual*/ RayTracerPlugin::ImageGenerator::~ImageGenerator( void )
{
	delete scene;
}

//===========================================================================
/*virtual*/ bool RayTracerPlugin::ImageGenerator::GeneratePixel( const wxPoint& point, const wxSize& size, wxColour& color )
{
	const View& view = plugin->view;

	// Make a right-handed coordinate frame from our viewing parameters.
	c3ga::vectorE3GA zAxis = -view.direction;
	c3ga::vectorE3GA yAxis = c3ga::unit( c3ga::lc( view.direction, c3ga::op( view.direction, view.up ) ) );
	c3ga::vectorE3GA xAxis = c3ga::lc( c3ga::op( zAxis, yAxis ), c3ga::trivectorE3GA( c3ga::trivectorE3GA::coord_e1e2e3, 1.0 ) );

	// Calculate our viewing window on the viewing plane.
	double xMax = view.focalLength * tan( view.angle * 0.5 );
	double xMin = -xMax;
	double yMax = xMax * double( size.y ) / double( size.x );
	double yMin = -yMax;

	// Calculate the projector ray from the eye to the given pixel.
	double xLerp = double( point.x ) / double( size.x - 1 );
	double yLerp = 1.0 - double( point.y ) / double( size.y - 1 );
	c3ga::vectorE3GA pixelPoint( c3ga::vectorE3GA::coord_e1_e2_e3,
									xMin + xLerp * ( xMax - xMin ),
									yMin + yLerp * ( yMax - yMin ),
									-view.focalLength );
	pixelPoint = view.eye +
					pixelPoint.get_e1() * xAxis +
					pixelPoint.get_e2() * yAxis +
					pixelPoint.get_e3() * zAxis;
	Scene::Ray ray;
	ray.point = view.eye;
	ray.direction = c3ga::unit( pixelPoint - ray.point );
	
	// Now go determine what light is visible while looking along that ray into the scene!
	c3ga::vectorE3GA visibleLight;
	scene->CalculateVisibleLight( ray, visibleLight );
	color.SetRGB(
			( wxUint32( 255.0 * visibleLight.get_e1() ) << 0 ) |
			( wxUint32( 255.0 * visibleLight.get_e2() ) << 8 ) |
			( wxUint32( 255.0 * visibleLight.get_e3() ) << 16 ) );

	return true;
}

//===========================================================================
/*virtual*/ igPlugin::ImageGenerator* RayTracerPlugin::NewImageGenerator( void )
{
	ImageGenerator* imageGenerator = new ImageGenerator();
	
	imageGenerator->plugin = this;

	// We give each image generator its own copy of the entire scene for thread-safety purposes.
	imageGenerator->scene = scene->Clone();

	return imageGenerator;
}

//===========================================================================
/*virtual*/ void RayTracerPlugin::DeleteImageGenerator( igPlugin::ImageGenerator* imageGenerator )
{
	delete imageGenerator;
}

// RayTracer.cpp