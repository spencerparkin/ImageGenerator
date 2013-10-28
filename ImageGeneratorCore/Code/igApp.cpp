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
	imageSize.Set( 1024, 1024 );

	threadCount = 5;
}

//===========================================================================
/*virtual*/ igApp::~igApp( void )
{
}

//===========================================================================
/*virtual*/ void igApp::OnInitCmdLine( wxCmdLineParser& parser )
{
	parser.AddOption( "plugin", wxEmptyString, "Load the given plugin upon application start-up.", wxCMD_LINE_VAL_STRING );
}

//===========================================================================
/*virtual*/ bool igApp::OnCmdLineParsed( wxCmdLineParser& parser )
{
	wxString pluginPath;
	if( parser.Found( "plugin", &pluginPath ) )
		LoadPlugin( pluginPath );
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
/*virtual*/ bool igApp::OnInit( void )
{
	if( !wxApp::OnInit() )
		return false;

	wxInitAllImageHandlers();
	
	igFrame* frame = new igFrame();
	frame->UpdateTitle();
	frame->Show();

	return true;
}

//===========================================================================
/*virtual*/ int igApp::OnExit( void )
{
	if( plugin || pluginHandle != NULL )
		UnloadPlugin();

	DeleteImage();

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

		if( !plugin->Initialize() )
			break;

		success = true;
	}
	while( false );

	if( !success )
		UnloadPlugin();

	return success;
}

//===========================================================================
bool igApp::UnloadPlugin( void )
{
	if( plugin )
	{
		plugin->Finalize();

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

		imageData = new unsigned char[ imageSize.GetWidth() * imageSize.GetHeight() * COLOR_COMPONENTS_PER_PIXEL ];
		if( !imageData )
			break;

		image = new wxImage( imageSize, imageData, true );

		if( !plugin->PreImageGeneration( image ) )
			break;

		if( threadCount == 0 )
			threadCount = wxThread::GetCPUCount();

		igThread::Manager manager;
		if( !manager.GenerateImage( threadCount ) )
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