// igApp.cpp

#include "igHeader.h"

//===========================================================================
wxIMPLEMENT_APP( igApp );

//===========================================================================
igApp::igApp( void )
{
	plugin = 0;
	pluginHandle = NULL;
	
	image = 0;
	imageData = 0;

	options.threadCount = 5;
	options.imageSize.Set( 1024, 1024 );

	frameLayout.pos = wxDefaultPosition;
	frameLayout.size = wxDefaultSize;

	frame = 0;
}

//===========================================================================
/*virtual*/ igApp::~igApp( void )
{
}

//===========================================================================
/*virtual*/ void igApp::OnInitCmdLine( wxCmdLineParser& parser )
{
	parser.AddOption( "plugin", wxEmptyString, "Load the given plugin upon application start-up.", wxCMD_LINE_VAL_STRING );
	parser.AddSwitch( "noConfig", wxEmptyString, "Suppress application configuration restoration." );
}

//===========================================================================
/*virtual*/ bool igApp::OnCmdLineParsed( wxCmdLineParser& parser )
{
	wxString pluginPath;
	if( parser.Found( "plugin", &pluginPath ) )
		LoadPlugin( pluginPath );

	if( wxCMD_SWITCH_ON != parser.FoundSwitch( "noConfig" ) )
		RestoreConfiguration();

	return true;
}

//===========================================================================
igPlugin* igApp::Plugin( void )
{
	return plugin;
}

//===========================================================================
wxImage* igApp::Image( void )
{
	return image;
}

//===========================================================================
wxCriticalSection* igApp::ImageCriticalSection( void )
{
	return &imageCriticalSection;
}

//===========================================================================
const igApp::Options& igApp::GetOptions( void )
{
	return options;
}

//===========================================================================
void igApp::SetOptions( const Options& options )
{
	this->options = options;
}

//===========================================================================
void igApp::SaveConfiguration( void )
{
	wxRegConfig regConfig;

	regConfig.Write( "threadCount", options.threadCount );
	regConfig.Write( "imageWidth", options.imageSize.x );
	regConfig.Write( "imageHeight", options.imageSize.y );
	
	regConfig.Write( "framePosX", frameLayout.pos.x );
	regConfig.Write( "framePosY", frameLayout.pos.y );
	regConfig.Write( "frameSizeX", frameLayout.size.x );
	regConfig.Write( "frameSizeY", frameLayout.size.y );
}

//===========================================================================
void igApp::RestoreConfiguration( void )
{
	wxRegConfig regConfig;

	regConfig.Read( "threadCount", &options.threadCount, 0 );
	regConfig.Read( "imageWidth", &options.imageSize.x, 1024 );
	regConfig.Read( "imageHeight", &options.imageSize.y, 1024 );

	regConfig.Read( "framePosX", &frameLayout.pos.x, -1 );
	regConfig.Read( "framePosY", &frameLayout.pos.y, -1 );
	regConfig.Read( "frameSizeX", &frameLayout.size.x, -1 );
	regConfig.Read( "frameSizeY", &frameLayout.size.y, -1 );
}

//===========================================================================
/*virtual*/ bool igApp::OnInit( void )
{
	frame = new igFrame( frameLayout.pos, frameLayout.size );

	if( !wxApp::OnInit() )
		return false;

	wxInitAllImageHandlers();
	
	frame->UpdateTitle();
	frame->Show();

	return true;
}

//===========================================================================
/*virtual*/ int igApp::OnExit( void )
{
	if( plugin || pluginHandle != NULL )
		UnloadPlugin( true );

	DeleteImage();

	SaveConfiguration();

	return wxApp::OnExit();
}

//===========================================================================
bool igApp::LoadPlugin( const wxString& pluginPath )
{
	bool success = false;

	do
	{
		if( plugin || pluginHandle != NULL )
			break;

		pluginHandle = LoadLibraryA( ( const char* )pluginPath.c_str() );
		if( pluginHandle == NULL )
			break;

		igPlugin::NewImageGeneratorPluginFunc func = ( igPlugin::NewImageGeneratorPluginFunc )GetProcAddress( pluginHandle, "NewImageGeneratorPlugin" );
		if( !func )
			break;

		plugin = func();
		if( !plugin )
			break;

		wxMenuBar* menuBar = frame->GetMenuBar();
		if( !plugin->Initialize( menuBar ) )
			break;

		success = true;
	}
	while( false );

	if( !success )
		UnloadPlugin();

	return success;
}

//===========================================================================
bool igApp::UnloadPlugin( bool frameDeleted /*= false*/ )
{
	if( plugin )
	{
		wxMenuBar* menuBar = frameDeleted ? 0 : frame->GetMenuBar();
		plugin->Finalize( menuBar );

		if( pluginHandle != NULL )
		{
			igPlugin::DeleteImageGeneratorPluginFunc func = ( igPlugin::DeleteImageGeneratorPluginFunc )GetProcAddress( pluginHandle, "DeleteImageGeneratorPlugin" );
			if( func )
			{
				func( plugin );
				plugin = 0;
			}
		}
	}

	if( plugin )
	{
		delete plugin;
		plugin = 0;
	}

	if( pluginHandle != NULL )
	{
		FreeLibrary( pluginHandle );
		pluginHandle = NULL;
	}

	return true;
}

//===========================================================================
bool igApp::DeleteImage( void )
{
	if( image )
	{
		delete image;
		image = 0;
	}

	if( imageData )
	{
		delete[] imageData;
		imageData = 0;
	}

	return true;
}

//===========================================================================
bool igApp::GenerateImage( void )
{
	bool success = false;

	do
	{
		if( !plugin )
			break;

		DeleteImage();

		imageData = new unsigned char[ options.imageSize.GetWidth() * options.imageSize.GetHeight() * COMPONENTS_PER_PIXEL ];
		if( !imageData )
			break;

		image = new wxImage( options.imageSize, imageData, true );

		if( !plugin->PreImageGeneration( image ) )
			break;

		igThread::Manager manager;
		if( !manager.GenerateImage( options.threadCount ) )
			break;

		if( !plugin->PostImageGeneration( image ) )
			break;

		success = true;
	}
	while( false );

	if( !success )
		DeleteImage();

	return success;
}

// igApp.cpp