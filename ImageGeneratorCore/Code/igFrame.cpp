// igFrame.cpp

#include "igHeader.h"

//===========================================================================
igFrame::igFrame( void ) : wxFrame( 0, wxID_ANY, "Image Generator", wxDefaultPosition, wxSize( 512, 512 ) )
{
	wxMenu* programMenu = new wxMenu();
	wxMenuItem* loadPluginMenuItem = new wxMenuItem( programMenu, ID_LoadPlugin, "Load Plugin", "Load an image generator plugin." );
	wxMenuItem* unloadPluginMenuItem = new wxMenuItem( programMenu, ID_UnloadPlugin, "Unload Plugin", "Unload the currently loaded image generator plugin, if any." );
	wxMenuItem* generateImageMenuItem = new wxMenuItem( programMenu, ID_GenerateImage, "Generate Image", "Use the currently loaded image generator plugin, if any, to generate an image." );
	wxMenuItem* saveImageMenuItem = new wxMenuItem( programMenu, ID_SaveImage, "Save Image", "Save the currently generated image, if any." );
	wxMenuItem* exitMenuItem = new wxMenuItem( programMenu, ID_Exit, "Exit", "Exit this program." );
	programMenu->Append( loadPluginMenuItem );
	programMenu->Append( unloadPluginMenuItem );
	programMenu->AppendSeparator();
	programMenu->Append( generateImageMenuItem );
	programMenu->Append( saveImageMenuItem );
	programMenu->AppendSeparator();
	programMenu->Append( exitMenuItem );

	wxMenu* helpMenu = new wxMenu();
	wxMenuItem* aboutMenuItem = new wxMenuItem( helpMenu, ID_About, "About", "Show the about-box." );
	helpMenu->Append( aboutMenuItem );

	wxMenuBar* menuBar = new wxMenuBar();
	menuBar->Append( programMenu, "Program" );
	menuBar->Append( helpMenu, "Help" );
	SetMenuBar( menuBar );

	wxStatusBar* statusBar = new wxStatusBar( this );
	SetStatusBar( statusBar );

	Bind( wxEVT_MENU, &igFrame::OnLoadPlugin, this, ID_LoadPlugin );
	Bind( wxEVT_MENU, &igFrame::OnUnloadPlugin, this, ID_UnloadPlugin );
	Bind( wxEVT_MENU, &igFrame::OnGenerateImage, this, ID_GenerateImage );
	Bind( wxEVT_MENU, &igFrame::OnSaveImage, this, ID_SaveImage );
	Bind( wxEVT_MENU, &igFrame::OnExit, this, ID_Exit );
	Bind( wxEVT_MENU, &igFrame::OnAbout, this, ID_About );
	Bind( wxEVT_UPDATE_UI, &igFrame::OnUpdateMenuItemUI, this, ID_LoadPlugin );
	Bind( wxEVT_UPDATE_UI, &igFrame::OnUpdateMenuItemUI, this, ID_UnloadPlugin );
	Bind( wxEVT_UPDATE_UI, &igFrame::OnUpdateMenuItemUI, this, ID_GenerateImage );
	Bind( wxEVT_UPDATE_UI, &igFrame::OnUpdateMenuItemUI, this, ID_SaveImage );
}

//===========================================================================
/*virtual*/ igFrame::~igFrame( void )
{
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
}

//===========================================================================
void igFrame::OnUnloadPlugin( wxCommandEvent& event )
{
	if( !wxGetApp().Plugin() )
		return;

	wxGetApp().UnloadPlugin();
}

//===========================================================================
void igFrame::OnGenerateImage( wxCommandEvent& event )
{
	if( !wxGetApp().GenerateImage() )
		wxMessageBox( "Failed to generate image." );
}

//===========================================================================
void igFrame::OnSaveImage( wxCommandEvent& event )
{
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
		{
			event.Enable( wxGetApp().Plugin() ? true : false );
			break;
		}
		case ID_SaveImage:
		{
			event.Enable( wxGetApp().Image() ? true : false );
			break;
		}
	}
}

//===========================================================================
void igFrame::OnExit( wxCommandEvent& event )
{
	Close( true );
}

//===========================================================================
void igFrame::OnAbout( wxCommandEvent& event )
{
}

// igFrame.cpp