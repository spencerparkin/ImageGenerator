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
bool igApp::GenerateImage( int frameIndex /*= 0*/, bool giveProgress /*= true*/, bool animating /*= false*/ )
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

		igPlugin::AnimationData animationData;
		animationData.animating = animating;
		animationData.frameIndex = frameIndex;
		animationData.frameCount = options.frameCount;
		animationData.frameRate = options.frameRate;

		if( !plugin->PreImageGeneration( image, &animationData ) )
			break;

		igThread::Manager manager;
		if( !manager.GenerateImage( options.threadCount, &animationData, 100, giveProgress ) )
			break;

		if( !plugin->PostImageGeneration( image, &animationData ) )
			break;

		success = true;
	}
	while( false );

	if( !success )
		DeleteImage();

	return success;
}

#ifdef VIDEO_SUPPORT

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

		AVCodecID codecId = AV_CODEC_ID_MPEG4; //AV_CODEC_ID_H264; //AV_CODEC_ID_MPEG1VIDEO;
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
		codecContext->gop_size = 10;		// Huh?!
		codecContext->max_b_frames = 1;		// Wha?!
		codecContext->pix_fmt = AV_PIX_FMT_YUV420P;
	
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
			if( !GenerateImage( frameIndex, false, true ) )
				break;

			if( !StuffImageInFrame( frame, codecContext->pix_fmt ) )
				break;

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
				int packetSize = packet.size;

				av_packet_unref( &packet );

				if( packetSize != bytesWritten )
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

bool igApp::StuffImageInFrame( AVFrame* frame, int pix_fmt )
{
	switch( pix_fmt )
	{
		// This is painful, but supposedly it compressed better in this format.
		///< planar YUV 4:2:0, 12bpp, (1 Cr & Cb sample per 2x2 Y samples)
		case AV_PIX_FMT_YUV420P:
		{
			if( frame->width != image->GetWidth() )
				return false;
			if( frame->linesize[0] != image->GetHeight() )
				return false;
			if( frame->linesize[1] != image->GetHeight() / 2 )
				return false;
			if( frame->linesize[2] != image->GetHeight() / 2 )
				return false;

			for( int i = 0; i < image->GetWidth(); i++ )
			{
				for( int j = 0; j < image->GetHeight(); j++ )
				{
					float r, g, b;
					float y, u, v;

					GetSample( i, j, r, g, b );
					ConvertSampleRGBtoYUV( r, g, b, y, u, v );

					unsigned char yByte = y * 255.f;
					frame->data[0][ j * frame->linesize[0] + i ] = yByte;
				}
			}

			for( int i = 0; i < image->GetWidth() / 2; i++ )
			{
				for( int j = 0; j < image->GetHeight() / 2; j++ )
				{
					float r[4], g[4], b[4];

					GetSample( 2*i, 2*j, r[0], g[0], b[0] );
					GetSample( 2*i + 1, 2*j, r[1], g[1], b[1] );
					GetSample( 2*i, 2*j + 1, r[2], g[2], b[2] );
					GetSample( 2*i + 1, 2*j + 1, r[3], g[3], b[3] );

					float rAvg = ( r[0] + r[1] + r[2] + r[3] ) / 4.f;
					float gAvg = ( g[0] + g[1] + g[2] + g[3] ) / 4.f;
					float bAvg = ( b[0] + b[1] + b[2] + b[3] ) / 4.f;
					
					float y, u, v;
					ConvertSampleRGBtoYUV( rAvg, gAvg, bAvg, y, u, v );

					unsigned char uByte = u * 255.f;
					unsigned char vByte = v * 255.f;
					frame->data[1][ j * frame->linesize[1] + i ] = uByte;
					frame->data[2][ j * frame->linesize[2] + i ] = vByte;
				}
			}

			return true;
		}
	}

	return false;
}

bool igApp::GetSample( int i, int j, float& r, float& g, float& b )
{
	if( i < 0 || i > image->GetWidth() - 1 )
		return false;
	if( j < 0 || j > image->GetHeight() - 1 )
		return false;

	r = float( image->GetRed( i, j ) ) / 255.f;
	g = float( image->GetGreen( i, j ) ) / 255.f;
	b = float( image->GetBlue( i, j ) ) / 255.f;

	return true;
}

void igApp::ConvertSampleRGBtoYUV( float r, float g, float b, float& y, float& u, float& v )
{
	float m[3][3];

	/*
	m[0][0] = 0.2126f;
	m[0][1] = 0.7152f;
	m[0][2] = 0.0722f;

	m[1][0] = -0.09991f;
	m[1][1] = -0.33609f;
	m[1][2] = 0.436f;

	m[2][0] = 0.615f;
	m[2][1] = -0.55861f;
	m[2][2] = -0.05639f;
	*/

	m[0][0] = 0.299f;
	m[0][1] = 0.587f;
	m[0][2] = 0.114f;

	m[1][0] = -0.14713f;
	m[1][1] = -0.28886f;
	m[1][2] = 0.436f;

	m[2][0] = 0.615f;
	m[2][1] = -0.51499f;
	m[2][2] = -0.10001f;

	y = m[0][0] * r + m[0][1] * g + m[0][2] * b;
	u = m[1][0] * r + m[1][1] * g + m[1][2] * b + 0.5f;
	v = m[2][0] * r + m[2][1] * g + m[2][2] * b + 0.5f;

	if( y < 0.f )	y = 0.f;
	if( y > 1.f )	y = 1.f;
	if( u < 0.f )	u = 0.f;
	if( u > 1.f )	u = 1.f;
	if( v < 0.f )	v = 0.f;
	if( v > 1.f )	v = 1.f;
}

#endif //VIDEO_SUPPORT

// igApp.cpp