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
RayTracerPlugin::RayTracerPlugin( void )
{
	scene = 0;
}

//===========================================================================
/*virtual*/ RayTracerPlugin::~RayTracerPlugin( void )
{
}

//===========================================================================
/*virtual*/ bool RayTracerPlugin::Initialize( void )
{
	scene = new Scene();

	// TODO: Populate the scene here with lights and objects.

	//view.eye.set( c3ga::vectorE3GA::coord_e1_e2_e3, 30.0, 30.0, 30.0 );
	view.eye.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 0.0, -30.0 );
	view.up.set( c3ga::vectorE3GA::coord_e1_e2_e3, 0.0, 1.0, 0.0 );
	view.direction = c3ga::unit( -view.eye );
	view.angle = M_PI / 5.0;
	view.focalLength = 10.0;

	return true;
}

//===========================================================================
/*virtual*/ bool RayTracerPlugin::Finalize( void )
{
	delete scene;
	scene = 0;

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
	c3ga::vectorE3GA xAxis = c3ga::rc( c3ga::trivectorE3GA( c3ga::trivectorE3GA::coord_e1e2e3, 1.0 ), c3ga::op( yAxis, zAxis ) );

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
	
	// Now go determine what color is visible while looking along that ray into the scene!
	c3ga::vectorE3GA visibleColor;
	scene->CalculateVisibleColor( ray, visibleColor );
	color.SetRGB(
			( wxUint32( 255.0 * visibleColor.get_e1() ) << 0 ) |
			( wxUint32( 255.0 * visibleColor.get_e2() ) << 8 ) |
			( wxUint32( 255.0 * visibleColor.get_e3() ) << 16 ) );

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