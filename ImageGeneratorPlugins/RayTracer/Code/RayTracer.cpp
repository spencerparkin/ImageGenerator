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
}

//===========================================================================
/*virtual*/ RayTracerPlugin::~RayTracerPlugin( void )
{
}

//===========================================================================
/*virtual*/ bool RayTracerPlugin::Initialize( void )
{
	return true;
}

//===========================================================================
/*virtual*/ bool RayTracerPlugin::Finalize( void )
{
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
}

//===========================================================================
/*virtual*/ RayTracerPlugin::ImageGenerator::~ImageGenerator( void )
{
}

//===========================================================================
/*virtual*/ bool RayTracerPlugin::ImageGenerator::GeneratePixel( const wxPoint& point, const wxSize& size, wxColour& color )
{
	color.SetRGB(0);
	return true;
}

//===========================================================================
/*virtual*/ igPlugin::ImageGenerator* RayTracerPlugin::NewImageGenerator( void )
{
	return new ImageGenerator();
}

//===========================================================================
/*virtual*/ void RayTracerPlugin::DeleteImageGenerator( igPlugin::ImageGenerator* imageGenerator )
{
	delete imageGenerator;
}

// RayTracer.cpp