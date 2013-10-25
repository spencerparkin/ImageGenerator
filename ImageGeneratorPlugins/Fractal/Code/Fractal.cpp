// Fractal.cpp

#include "Fractal.h"

//===========================================================================
extern "C" __declspec( dllexport ) FractalPlugin* NewImageGeneratorPlugin( void )
{
	return new FractalPlugin();
}

//===========================================================================
extern "C" __declspec( dllexport ) void DeleteImageGeneratorPlugin( igPlugin* igPlugin )
{
	delete igPlugin;
}

//===========================================================================
FractalPlugin::FractalPlugin( void )
{
}

//===========================================================================
/*virtual*/ FractalPlugin::~FractalPlugin( void )
{
}

//===========================================================================
/*virtual*/ bool FractalPlugin::Initialize( void )
{
	return true;
}

//===========================================================================
/*virtual*/ bool FractalPlugin::Finalize( void )
{
	return true;
}

//===========================================================================
/*virtual*/ igPlugin::ImageGenerator* FractalPlugin::NewImageGenerator( void )
{
	return new ImageGenerator( this );
}

//===========================================================================
/*virtual*/ void FractalPlugin::DeleteImageGenerator( igPlugin::ImageGenerator* imageGenerator )
{
	delete imageGenerator;
}

//===========================================================================
/*virtual*/ bool FractalPlugin::PreImageGeneration( wxImage* image )
{
	return true;
}

//===========================================================================
/*virtual*/ bool FractalPlugin::PostImageGeneration( wxImage* image )
{
	return true;
}

//===========================================================================
FractalPlugin::ImageGenerator::ImageGenerator( FractalPlugin* fractalPlugin )
{
	this->fractalPlugin = fractalPlugin;
}

//===========================================================================
/*virtual*/ FractalPlugin::ImageGenerator::~ImageGenerator( void )
{
}

//===========================================================================
/*virtual*/ bool FractalPlugin::ImageGenerator::GeneratePixel( const wxPoint& point, const wxSize& size, wxColour& color )
{
	color.Set( 0, 0, 0 );
	return true;
}

// Fractal.cpp