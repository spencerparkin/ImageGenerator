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
	options.frameCount = 0;
	options.frameRate = 30;

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
	regConfig.Write( "frameCount", options.frameCount );
	regConfig.Write( "frameRate", options.frameRate );
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
	regConfig.Read( "frameCount", &options.frameCount, 0 );
	regConfig.Read( "frameRate", &options.frameRate, 30 );
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
	frame = new igFrame();

	if( !wxApp::OnInit() )
		return false;

	wxInitAllImageHandlers();
	
	frame->SetPosition( frameLayout.pos );
	frame->SetSize( frameLayout.size );
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
		if( !plugin->Initialize( menuBar, frame ) )
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
		plugin->Finalize( menuBar, ( frameDeleted ? 0 : frame ) );

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
bool igApp::GenerateImage( int frameIndex /*= 0*/, bool giveProgress /*= true*/ )
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
		if( !manager.GenerateImage( options.threadCount, frameIndex, options.frameCount, 100, giveProgress ) )
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

bool igApp::GenerateVideo( const wxString& videoPath )
{
	bool success = false;
	wxProgressDialog* progressDialog = nullptr;
	AVCodec* codec = nullptr;
	AVCodecContext* codecContext = nullptr;
	wxFile videoFile;
	AVFrame* frame = nullptr;
	int ret = 0;

	do
	{
		if( options.frameCount == 0 )
			break;

		avcodec_register_all();

		AVCodecID codecId = AV_CODEC_ID_MPEG1VIDEO;
		codec = avcodec_find_encoder( codecId );
		if( !codec )
		{
			wxMessageBox( "Failed to find codec for MPEG format." );
			break;
		}

		codecContext = avcodec_alloc_context3( codec );
		if( !codecContext )
		{
			wxMessageBox( "Failed to allocate codec context." );
			break;
		}

		codecContext->bit_rate = 400000;		// What is this?
		codecContext->width = options.imageSize.GetWidth();
		codecContext->height = options.imageSize.GetHeight();
		codecContext->time_base.num = 1;
		codecContext->time_base.den = options.frameRate;
		//codecContext->gop_size = 10;		// Huh?!
		//codecContext->max_b_frames = 1;		// Wha?!
		codecContext->pix_fmt = AV_PIX_FMT_ABGR;
	
		ret = avcodec_open2( codecContext, codec, NULL );
		if( ret < 0 )
		{
			wxMessageBox( "Failed to open codec: " + GetFFMpegError( ret ) );
			break;
		}

		frame = av_frame_alloc();
		frame->width = codecContext->width;
		frame->height = codecContext->height;

		ret = av_image_alloc( frame->data, frame->linesize, frame->width, frame->height, codecContext->pix_fmt, 32 );
		if( ret < 0 )
		{
			wxMessageBox( "Failed to allocate frame: " + GetFFMpegError( ret ) );
			break;
		}

		if( !videoFile.Open( videoPath, wxFile::write ) )
		{
			wxMessageBox( "Failed to open file \"" + videoPath + "\" for writing." );
			break;
		}
		
		progressDialog = new wxProgressDialog(
							"Video Generation In Progress", "Generating Video...",
							options.frameCount, 0,
							wxPD_APP_MODAL | wxPD_CAN_ABORT | wxPD_ESTIMATED_TIME | wxPD_AUTO_HIDE );

		AVPacket packet;
		
		int frameIndex;
		for( frameIndex = 0; frameIndex < options.frameCount; frameIndex++ )
		{
			if( !GenerateImage( frameIndex, false ) )
				break;

			// Copy the generated image into the target format for the video.
			for( int i = 0; i < frame->width; i++ )
			{
				for( int j = 0; j < frame->height; j++ )
				{
					unsigned char r = image->GetRed( i, j );
					unsigned char g = image->GetGreen( i, j );
					unsigned char b = image->GetBlue( i, j );
					unsigned char a = image->GetAlpha( i, j );

					switch( codecContext->pix_fmt )
					{
						case AV_PIX_FMT_ABGR:
						{
							frame->data[0][ j * frame->linesize[0] + i ] = a;
							frame->data[1][ j * frame->linesize[1] + i ] = b;
							frame->data[2][ j * frame->linesize[2] + i ] = g;
							frame->data[3][ j * frame->linesize[3] + i ] = r;
							break;
						}
					}
				}
			}

			frame->pts = frameIndex;

			av_init_packet( &packet );
			packet.data = nullptr;
			packet.size = 0;

			// Encode the frame!
			int gotPacket = 0;
			ret = avcodec_encode_video2( codecContext, &packet, frame, &gotPacket );
			if( ret < 0 )
			{
				wxMessageBox( wxString::Format( "Failed to encode frame %d: ", frameIndex + 1 ) + GetFFMpegError( ret ) );
				break;
			}

			wxString progressMsg = wxString::Format( "Writing frame %d/%d...", frameIndex + 1, options.frameCount );
			if( gotPacket )
				progressMsg += wxString::Format( " (packet size = %5d)", packet.size );
			progressDialog->Update( frameIndex, progressMsg );

			if( gotPacket )
			{
				int bytesWritten = videoFile.Write( packet.data, packet.size );
				av_packet_unref( &packet );

				if( packet.size != bytesWritten )
				{
					wxMessageBox( "Failed to write packet!" );
					break;
				}

				videoFile.Flush();
			}

			if( progressDialog->WasCancelled() )
				break;
		}

		if( frameIndex < options.frameCount )
			break;

		// Now encode the delayed frames.
		while( true )
		{
			int gotPacket = 0;
			ret = avcodec_encode_video2( codecContext, &packet, NULL, &gotPacket );
			if( ret < 0 )
			{
				wxMessageBox( "Failed to encode delay frame: " + GetFFMpegError( ret ) );
				break;
			}

			if( !gotPacket )
				break;
			else
			{
				videoFile.Write( packet.data, packet.size );
				videoFile.Flush();
				av_packet_unref( &packet );
			}
		}

		success = true;
	}
	while( false );

	delete progressDialog;

	if( success )
	{
		uint8_t endcode[] = { 0, 0, 1, 0xb7 };
		videoFile.Write( endcode, sizeof( endcode ) );
	}

	videoFile.Close();

	if( frame )
	{
		av_freep( &frame->data[0] );
		av_frame_free( &frame );
	}

	if( codecContext )
	{
		avcodec_close( codecContext );
		av_free( codecContext );
	}

	return success;
}

wxString igApp::GetFFMpegError( int ret )
{
	wxString error = "Unknown";
	char errbuf[ 1024 ];
	if( av_strerror( ret, errbuf, sizeof( errbuf ) ) == 0 )
		error = errbuf;
	return error;
}

// igApp.cpp