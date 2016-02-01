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
FractalPlugin::FractalPlugin( void ) : menuEventHandler( this )
{
	colorTable = 0;
	colorTableSize = 0;

	ResetRegion();
	SetTargetRegion();

	maxIters = 5000;
}

//===========================================================================
/*virtual*/ FractalPlugin::~FractalPlugin( void )
{
}

//===========================================================================
/*virtual*/ bool FractalPlugin::Initialize( wxMenuBar* menuBar, wxEvtHandler* updateUIHandler )
{
	GenerateColorTable();

	menuEventHandler.InsertMenu( menuBar, updateUIHandler );

	return true;
}

//===========================================================================
/*virtual*/ bool FractalPlugin::Finalize( wxMenuBar* menuBar, wxEvtHandler* updateUIHandler )
{
	delete[] colorTable;
	colorTable = 0;
	colorTableSize = 0;

	menuEventHandler.RemoveMenu( menuBar, updateUIHandler );

	return true;
}

//===========================================================================
/*virtual*/ wxString FractalPlugin::Name( void )
{
	return "Fractal";
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
/*virtual*/ bool FractalPlugin::PreImageGeneration( wxImage* image, int frameIndex, int frameCount, bool animating )
{
	if( animating )
	{
		if( frameIndex == 0 )
			region = regionSource;
		else
		{
			long double eps = 1e-5;

			long double realMinPhi = CalcPhi( regionSource.realMin, regionTarget.realMin, eps, frameCount );
			long double realMaxPhi = CalcPhi( regionSource.realMax, regionTarget.realMax, eps, frameCount );
			long double imagMinPhi = CalcPhi( regionSource.imagMin, regionTarget.imagMin, eps, frameCount );
			long double imagMaxPhi = CalcPhi( regionSource.imagMax, regionTarget.imagMax, eps, frameCount );

			region.realMin = ApplyPhi( regionSource.realMin, regionTarget.realMin, realMinPhi, frameIndex );
			region.realMax = ApplyPhi( regionSource.realMax, regionTarget.realMax, realMaxPhi, frameIndex );
			region.imagMin = ApplyPhi( regionSource.imagMin, regionTarget.imagMin, imagMinPhi, frameIndex );
			region.imagMax = ApplyPhi( regionSource.imagMax, regionTarget.imagMax, imagMaxPhi, frameIndex );
		}
	}

	return true;
}

//===========================================================================
long double FractalPlugin::CalcPhi( long double A, long double B, long double eps, int frameCount )
{
	/*
	long double ln_eps = log( eps );
	long double ln_A_to_B = log( fabs( A - B ) );
	long double ln_frameCount = log( long double( frameCount ) );
	long double phi = ( ln_eps - ln_A_to_B ) / ln_frameCount;
	*/

	long double phi = 0.95;
	while( true )
	{
		eps = pow( phi, long double( frameCount ) ) * fabs( A - B );
		if( eps > 1e-8 )
			phi *= 0.99;
		else
			break;
	}

	return phi;
}

//===========================================================================
long double FractalPlugin::ApplyPhi( long double A, long double B, long double phi, int frameIndex )
{
	long double lerp = pow( phi, long double( frameIndex ) );
	long double result = lerp * A + ( 1.0 - lerp ) * B;
	return result;
}

//===========================================================================
/*virtual*/ bool FractalPlugin::PostImageGeneration( wxImage* image, int frameIndex, int frameCount, bool animating )
{
	return true;
}

//===========================================================================
/*virtual*/ bool FractalPlugin::SubregionSelect( const wxRect& rect, const wxSize& size )
{
	long double realMinLerp = long double( rect.x ) / long double( size.x );
	long double realMaxLerp = long double( rect.x + rect.width ) / long double( size.x );

	long double imagMinLerp = 1.0 - long double( rect.y + rect.height ) / long double( size.y );
	long double imagMaxLerp = 1.0 - long double( rect.y ) / long double( size.y );

	long double realDelta = region.realMax - region.realMin;
	region.realMax = region.realMin + realMaxLerp * realDelta;
	region.realMin = region.realMin + realMinLerp * realDelta;

	long double imagDelta = region.imagMax - region.imagMin;
	region.imagMax = region.imagMin + imagMaxLerp * imagDelta;
	region.imagMin = region.imagMin + imagMinLerp * imagDelta;

	// TODO: Fix the aspect ratio here of our window into the complex plane
	//       so that it matches that of the given image size.

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

	long double realZ = 0.0;
	long double imagZ = 0.0;
	long double realC = fractalPlugin->region.realMin + long double( point.x ) / long double( size.x ) * ( fractalPlugin->region.realMax - fractalPlugin->region.realMin );
	long double imagC = fractalPlugin->region.imagMin + ( 1.0 - long double( point.y ) / long double( size.y ) ) * ( fractalPlugin->region.imagMax - fractalPlugin->region.imagMin );

	int i;
	for( i = 0; i < fractalPlugin->maxIters; i++ )
	{
		// Calculate Z <- Z^2 + C.
		long double real = realZ * realZ - imagZ * imagZ + realC;
		long double imag = 2.0 * realZ * imagZ + imagC;
		realZ = real;
		imagZ = imag;

		// It can be shown that if the magnitude of Z ever becomes greater than 2,
		// then we have escaped the Mandelbrot set.
		long double squareMagnitude = realZ * realZ + imagZ * imagZ;
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

//===========================================================================
void FractalPlugin::ResetRegion( void )
{
	region.realMin = -2.5;
	region.realMax = 2.5;
	region.imagMin = -2.5;
	region.imagMax = 2.5;
}

//===========================================================================
void FractalPlugin::SetTargetRegion( void )
{
	regionTarget = region;

	ResetRegion();

	regionSource = region;
}

//===========================================================================
FractalPlugin::MenuEventHandler::MenuEventHandler( FractalPlugin* fractalPlugin )
{
	this->fractalPlugin = fractalPlugin;

	ID_SetTargetRegion = -1;
}

//===========================================================================
/*virtual*/ FractalPlugin::MenuEventHandler::~MenuEventHandler( void )
{
}

//===========================================================================
void FractalPlugin::MenuEventHandler::InsertMenu( wxMenuBar* menuBar, wxEvtHandler* updateUIHandler )
{
	int maxId = CalcMaxMenuId( menuBar );

	ID_ResetRegion = ++maxId;
	ID_SetTargetRegion = ++maxId;

	wxMenu* fractalMenu = new wxMenu();
	wxMenuItem* resetRegionMenuItem = new wxMenuItem( fractalMenu, ID_ResetRegion, "Reset Region", "Reset the region we view into the complex plane." );
	wxMenuItem* setTargetRegionMenuItem = new wxMenuItem( fractalMenu, ID_SetTargetRegion, "Set Target Region", "Set the target region from fractal zoom animation." );
	fractalMenu->Append( resetRegionMenuItem );
	fractalMenu->Append( setTargetRegionMenuItem );

	menuBar->Append( fractalMenu, "Fractal" );

	menuBar->Bind( wxEVT_MENU, &MenuEventHandler::OnResetRegion, this, ID_ResetRegion );
	menuBar->Bind( wxEVT_MENU, &MenuEventHandler::OnSetTargetRegion, this, ID_SetTargetRegion );
	updateUIHandler->Bind( wxEVT_UPDATE_UI, &MenuEventHandler::OnUpdateMenuItemUI, this, ID_SetTargetRegion );
}

//===========================================================================
void FractalPlugin::MenuEventHandler::RemoveMenu( wxMenuBar* menuBar, wxEvtHandler* updateUIHandler )
{
	if( updateUIHandler )
	{
		updateUIHandler->Unbind( wxEVT_UPDATE_UI, &MenuEventHandler::OnUpdateMenuItemUI, this, ID_SetTargetRegion );
	}

	if( menuBar )
	{
		int menuPos = menuBar->FindMenu( "Fractal" );
		if( menuPos != wxNOT_FOUND )
		{
			wxMenu* fractalMenu = menuBar->Remove( menuPos );
			delete fractalMenu;
		}
	}
}

//===========================================================================
void FractalPlugin::MenuEventHandler::OnResetRegion( wxCommandEvent& event )
{
	fractalPlugin->ResetRegion();

	wxMessageBox( "Region reset!" );
}

//===========================================================================
void FractalPlugin::MenuEventHandler::OnSetTargetRegion( wxCommandEvent& event )
{
	fractalPlugin->SetTargetRegion();

	wxMessageBox( wxString::Format( "Target region set!  [ rmin: %LG, rmax: %LG, imin: %LG, imax %LG ]",
					fractalPlugin->regionTarget.realMin,
					fractalPlugin->regionTarget.realMax,
					fractalPlugin->regionTarget.imagMin,
					fractalPlugin->regionTarget.imagMax ) );
}

//===========================================================================
void FractalPlugin::MenuEventHandler::OnUpdateMenuItemUI( wxUpdateUIEvent& event )
{
	event.Enable( true );
}

// Fractal.cpp