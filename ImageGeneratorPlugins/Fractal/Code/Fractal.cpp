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
	colorTable = 0;
	colorTableSize = 0;

	realMin = -2.5;
	realMax = 2.5;
	imagMin = -2.5;
	imagMax = 2.5;

	maxIters = 500;
}

//===========================================================================
/*virtual*/ FractalPlugin::~FractalPlugin( void )
{
}

//===========================================================================
/*virtual*/ bool FractalPlugin::Initialize( void )
{
	GenerateColorTable();
	return true;
}

//===========================================================================
/*virtual*/ bool FractalPlugin::Finalize( void )
{
	delete[] colorTable;
	colorTable = 0;
	colorTableSize = 0;
	return true;
}

//===========================================================================
void FractalPlugin::GenerateColorTable( void )
{
	wxColour prominantColor[6];
	prominantColor[0].SetRGB( 0xFF0000 );
	prominantColor[1].SetRGB( 0xFFFF00 );
	prominantColor[2].SetRGB( 0x00FF00 );
	prominantColor[3].SetRGB( 0x00FFFF );
	prominantColor[4].SetRGB( 0x0000FF );
	prominantColor[5].SetRGB( 0xFF00FF );
	int prominantColorCount = sizeof( prominantColor ) / sizeof( wxColour );

	int colorShadeCount = 8;
	colorTableSize = prominantColorCount * colorShadeCount;
	colorTable = new wxColour[ colorTableSize ];

	wxColour* color = colorTable;
	for( int i = 0; i < prominantColorCount; i++ )
	{
		wxColor* color0 = &prominantColor[ i ];
		wxColor* color1 = &prominantColor[ ( i + 1 ) % prominantColorCount ];
		for( int j = 0; j < colorShadeCount; j++ )
		{
			float t = float( j ) / float( colorShadeCount );
			float r = float( color0->Red() ) * ( 1.f - t ) + float( color1->Red() ) * t;
			float g = float( color0->Green() ) * ( 1.f - t ) + float( color1->Green() ) * t;
			float b = float( color0->Blue() ) * ( 1.f - t ) + float( color1->Blue() ) * t;

			( color++ )->SetRGB(
					( unsigned char( r ) << 0 ) |
					( unsigned char( g ) << 8 ) |
					( unsigned char( b ) << 16 ) );
		}
	}
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
	// Assume that the given point is a member of the set.
	// All members of the set are colored black in the complex plane.
	color.Set( 0, 0, 0 );

	double realZ = 0.0;
	double imagZ = 0.0;
	double realC = fractalPlugin->realMin + double( point.x ) / double( size.x ) * ( fractalPlugin->realMax - fractalPlugin->realMin );
	double imagC = fractalPlugin->imagMin + double( point.y ) / double( size.y ) * ( fractalPlugin->imagMax - fractalPlugin->imagMin );

	int i;
	for( i = 0; i < fractalPlugin->maxIters; i++ )
	{
		// Calculate Z <- Z^2 + C.
		double real = realZ * realZ - imagZ * imagZ + realC;
		double imag = 2.0 * realZ * imagZ + imagC;
		realZ = real;
		imagZ = imag;

		// It can be shown that if the magnitude of Z ever becomes greater than 2,
		// then we have escaped the Mandelbrot set.
		double squareMagnitude = realZ * realZ + imagZ * imagZ;
		if( squareMagnitude > 4.0 )
			break;
	}

	// Are we a member of the set?
	if( i < fractalPlugin->maxIters )
	{
		// We are not a member of the set.  Color the pixel something other than black.
		color = fractalPlugin->colorTable[ i % fractalPlugin->colorTableSize ];
	}

	return true;
}

// Fractal.cpp