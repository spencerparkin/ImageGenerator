// igFrame.cpp

#include "igHeader.h"

//===========================================================================
igFrame::igFrame( const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/ ) :
								wxFrame( 0, wxID_ANY, "Image Generator", pos, size )
{
	wxMenu* programMenu = new wxMenu();
	wxMenuItem* loadPluginMenuItem = new wxMenuItem( programMenu, ID_LoadPlugin, "Load Plugin", "Load an image generator plugin." );
	wxMenuItem* unloadPluginMenuItem = new wxMenuItem( programMenu, ID_UnloadPlugin, "Unload Plugin", "Unload the currently loaded image generator plugin, if any." );
	wxMenuItem* optionsMenuItem = new wxMenuItem( programMenu, ID_Options, "Options", "Show the options dialog box." );
	wxMenuItem* generateImageMenuItem = new wxMenuItem( programMenu, ID_GenerateImage, "Generate Image", "Use the currently loaded image generator plugin, if any, to generate an image." );
	wxMenuItem* saveImageMenuItem = new wxMenuItem( programMenu, ID_SaveImage, "Save Image", "Save the currently generated image, if any." );
	wxMenuItem* generateVideoMenuItem = new wxMenuItem( programMenu, ID_GenerateVideo, "Generate Video", "Use the currently loaded image generated plugin, if any, to generate a video." );
	wxMenuItem* exitMenuItem = new wxMenuItem( programMenu, ID_Exit, "Exit", "Exit this program." );
	programMenu->Append( loadPluginMenuItem );
	programMenu->Append( unloadPluginMenuItem );
	programMenu->AppendSeparator();
	programMenu->Append( optionsMenuItem );
	programMenu->AppendSeparator();
	programMenu->Append( generateImageMenuItem );
	programMenu->Append( saveImageMenuItem );
	programMenu->AppendSeparator();
	programMenu->Append( generateVideoMenuItem );
	programMenu->AppendSeparator();
	programMenu->Append( exitMenuItem );

	wxMenu* zoomMenu = new wxMenu();
	wxMenuItem* zoom25MenuItem = new wxMenuItem( zoomMenu, ID_Zoom25, "25%", "Show the image at 25% of its normal size.", wxITEM_CHECK );
	wxMenuItem* zoom50MenuItem = new wxMenuItem( zoomMenu, ID_Zoom50, "50%", "Show the image at 50% of its normal size.", wxITEM_CHECK );
	wxMenuItem* zoom100MenuItem = new wxMenuItem( zoomMenu, ID_Zoom100, "100%", "Show the image in its normal size.", wxITEM_CHECK );
	wxMenuItem* zoom150MenuItem = new wxMenuItem( zoomMenu, ID_Zoom150, "150%", "Show the image at 150% of its normal size.", wxITEM_CHECK );
	wxMenuItem* zoom200MenuItem = new wxMenuItem( zoomMenu, ID_Zoom200, "200%", "Show the image at 200% of its normal size.", wxITEM_CHECK );
	zoomMenu->Append( zoom25MenuItem );
	zoomMenu->Append( zoom50MenuItem );
	zoomMenu->Append( zoom100MenuItem );
	zoomMenu->Append( zoom150MenuItem );
	zoomMenu->Append( zoom200MenuItem );

	wxMenu* helpMenu = new wxMenu();
	wxMenuItem* aboutMenuItem = new wxMenuItem( helpMenu, ID_About, "About", "Show the about-box." );
	helpMenu->Append( aboutMenuItem );

	wxMenuBar* menuBar = new wxMenuBar();
	menuBar->Append( programMenu, "Program" );
	menuBar->Append( zoomMenu, "Zoom" );
	menuBar->Append( helpMenu, "Help" );
	SetMenuBar( menuBar );

	wxStatusBar* statusBar = new wxStatusBar( this );
	SetStatusBar( statusBar );

	canvas = new igCanvas( this );
	canvas->SetScrollRate( 1, 1 );

	wxBoxSizer* boxSizer = new wxBoxSizer( wxVERTICAL );
	boxSizer->Add( canvas, 1, wxEXPAND | wxALL, 0 );
	SetSizer( boxSizer );

	Bind( wxEVT_MENU, &igFrame::OnLoadPlugin, this, ID_LoadPlugin );
	Bind( wxEVT_MENU, &igFrame::OnUnloadPlugin, this, ID_UnloadPlugin );
	Bind( wxEVT_MENU, &igFrame::OnOptions, this, ID_Options );
	Bind( wxEVT_MENU, &igFrame::OnGenerateImage, this, ID_GenerateImage );
	Bind( wxEVT_MENU, &igFrame::OnSaveImage, this, ID_SaveImage );
	Bind( wxEVT_MENU, &igFrame::OnGenerateVideo, this, ID_GenerateVideo );
	Bind( wxEVT_MENU, &igFrame::OnZoom, this, ID_Zoom25 );
	Bind( wxEVT_MENU, &igFrame::OnZoom, this, ID_Zoom50 );
	Bind( wxEVT_MENU, &igFrame::OnZoom, this, ID_Zoom100 );
	Bind( wxEVT_MENU, &igFrame::OnZoom, this, ID_Zoom150 );
	Bind( wxEVT_MENU, &igFrame::OnZoom, this, ID_Zoom200 );
	Bind( wxEVT_MENU, &igFrame::OnExit, this, ID_Exit );
	Bind( wxEVT_MENU, &igFrame::OnAbout, this, ID_About );
	Bind( wxEVT_UPDATE_UI, &igFrame::OnUpdateMenuItemUI, this, ID_LoadPlugin );
	Bind( wxEVT_UPDATE_UI, &igFrame::OnUpdateMenuItemUI, this, ID_UnloadPlugin );
	Bind( wxEVT_UPDATE_UI, &igFrame::OnUpdateMenuItemUI, this, ID_GenerateImage );
	Bind( wxEVT_UPDATE_UI, &igFrame::OnUpdateMenuItemUI, this, ID_SaveImage );
	Bind( wxEVT_UPDATE_UI, &igFrame::OnUpdateMenuItemUI, this, ID_GenerateVideo );
	Bind( wxEVT_UPDATE_UI, &igFrame::OnUpdateMenuItemUI, this, ID_Zoom25 );
	Bind( wxEVT_UPDATE_UI, &igFrame::OnUpdateMenuItemUI, this, ID_Zoom50 );
	Bind( wxEVT_UPDATE_UI, &igFrame::OnUpdateMenuItemUI, this, ID_Zoom100 );
	Bind( wxEVT_UPDATE_UI, &igFrame::OnUpdateMenuItemUI, this, ID_Zoom150 );
	Bind( wxEVT_UPDATE_UI, &igFrame::OnUpdateMenuItemUI, this, ID_Zoom200 );
}

//===========================================================================
/*virtual*/ igFrame::~igFrame( void )
{
}

//===========================================================================
void igFrame::UpdateTitle( void )
{
	igPlugin* plugin = wxGetApp().Plugin();
	if( plugin )
		SetTitle( plugin->Name() + " -- Image Generator" );
	else
		SetTitle( "Image Generator" );
}

//===========================================================================
void igFrame::OnLoadPlugin( wxCommandEvent& event )
{
	if( wxGetApp().Plugin() )
		return;

	wxFileDialog fileDialog( this, "Open image generator plugin",
								wxEmptyString, wxEmptyString,
								"Image Generator Plugin (*.ig)|*.ig",
								wxFD_OPEN | wxFD_FILE_MUST_EXIST );

	if( fileDialog.ShowModal() == wxID_CANCEL )
		return;

	wxString pluginPath = fileDialog.GetPath();
	if( !wxGetApp().LoadPlugin( pluginPath ) )
		wxMessageBox( "Failed to load plugin \"" + pluginPath + "\"." );
	else
		UpdateTitle();
}

//===========================================================================
void igFrame::OnUnloadPlugin( wxCommandEvent& event )
{
	if( !wxGetApp().Plugin() )
		return;

	wxGetApp().UnloadPlugin();
	UpdateTitle();
}

//===========================================================================
void igFrame::OnOptions( wxCommandEvent& event )
{
	igOptionsDialog optionsDialog( this, wxGetApp().GetOptions() );
	if( wxID_OK == optionsDialog.ShowModal() )
		wxGetApp().SetOptions( optionsDialog.Options() );
}

//===========================================================================
void igFrame::OnGenerateImage( wxCommandEvent& event )
{
	if( !wxGetApp().GenerateImage() )
		wxMessageBox( "Failed to generate image." );
	else
	{
		canvas->InvalidateCachedBitmap();
		canvas->Refresh();
	}
}

//===========================================================================
void igFrame::OnSaveImage( wxCommandEvent& event )
{
	wxImage* image = wxGetApp().Image();
	if( !image )
		return;

	wxFileDialog fileDialog( this, "Save generated image",
								wxEmptyString, wxEmptyString,
								"PNG file (*.png)|*.png",
								wxFD_SAVE | wxFD_OVERWRITE_PROMPT );

	if( fileDialog.ShowModal() == wxID_CANCEL )
		return;

	wxString imagePath = fileDialog.GetPath();
	if( !image->SaveFile( imagePath, wxBITMAP_TYPE_PNG ) )
		wxMessageBox( "Failed to save image \"" + imagePath + "\"." );
}

//===========================================================================
void igFrame::OnGenerateVideo( wxCommandEvent& event )
{
#ifdef VIDEO_SUPPORT
	wxFileDialog fileDialog( this, "Save generated video",
								wxEmptyString, wxEmptyString,
								"MPEG file (*.mpeg)|*.mpeg",
								wxFD_SAVE | wxFD_OVERWRITE_PROMPT );

	if( fileDialog.ShowModal() == wxID_CANCEL )
		return;

	wxString videoPath = fileDialog.GetPath();
	if( !wxGetApp().GenerateVideo( videoPath ) )
		wxMessageBox( "Failed to generate video." );
#else
	wxMessageBox( "No video support compiled in." );
#endif //VIDEO_SUPPORT
}

//===========================================================================
void igFrame::OnZoom( wxCommandEvent& event )
{
	igCanvas::Zoom zoom = igCanvas::ZOOM_100;
	switch( event.GetId() )
	{
		case ID_Zoom25: zoom = igCanvas::ZOOM_25; break;
		case ID_Zoom50: zoom = igCanvas::ZOOM_50; break;
		case ID_Zoom100: zoom = igCanvas::ZOOM_100; break;
		case ID_Zoom150: zoom = igCanvas::ZOOM_150; break;
		case ID_Zoom200: zoom = igCanvas::ZOOM_200; break;
	}

	if( zoom != canvas->GetZoom() )
	{
		canvas->SetZoom( zoom );
		canvas->InvalidateCachedBitmap();
		canvas->Refresh();
	}
}

//===========================================================================
void igFrame::OnUpdateMenuItemUI( wxUpdateUIEvent& event )
{
	switch( event.GetId() )
	{
		case ID_LoadPlugin:
		{
			event.Enable( wxGetApp().Plugin() ? false : true );
			break;
		}
		case ID_UnloadPlugin:
		case ID_GenerateImage:
		case ID_GenerateVideo:
		{
#ifdef VIDEO_SUPPORT
			event.Enable( wxGetApp().Plugin() ? true : false );
#else
			event.Enable( false );
#endif //VIDEO_SUPPORT
			break;
		}
		case ID_SaveImage:
		{
			event.Enable( wxGetApp().Image() ? true : false );
			break;
		}
		case ID_Zoom25:
		case ID_Zoom50:
		case ID_Zoom100:
		case ID_Zoom150:
		case ID_Zoom200:
		{
			event.Enable( wxGetApp().Image() ? true : false );
			
			bool checked =
					( event.GetId() == ID_Zoom25 && canvas->GetZoom() == igCanvas::ZOOM_25 ) ||
					( event.GetId() == ID_Zoom50 && canvas->GetZoom() == igCanvas::ZOOM_50 ) ||
					( event.GetId() == ID_Zoom100 && canvas->GetZoom() == igCanvas::ZOOM_100 ) ||
					( event.GetId() == ID_Zoom150 && canvas->GetZoom() == igCanvas::ZOOM_150 ) ||
					( event.GetId() == ID_Zoom200 && canvas->GetZoom() == igCanvas::ZOOM_200 );

			event.Check( checked );

			break;
		}
	}
}

//===========================================================================
void igFrame::OnExit( wxCommandEvent& event )
{
	igApp::FrameLayout& frameLayout = wxGetApp().frameLayout;
	frameLayout.pos = GetPosition();
	frameLayout.size = GetSize();

	Close( true );
}

//===========================================================================
void igFrame::OnAbout( wxCommandEvent& event )
{
	wxAboutDialogInfo aboutDialogInfo;

	aboutDialogInfo.SetName( "Image Generator" );
	aboutDialogInfo.SetVersion( "v1.0" );
	aboutDialogInfo.SetDescription( "Generate an image on multiple threads." );
	aboutDialogInfo.SetCopyright( "Copyright (C) 2013, 2016, 2018" );
	aboutDialogInfo.AddDeveloper( "Spencer T. Parkin" );
	aboutDialogInfo.SetLicense( "Not licensed!" );

	wxAboutBox( aboutDialogInfo );
}

// igFrame.cpp