// igFrame.h

//===========================================================================
class igFrame : public wxFrame
{
public:

	igFrame( const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize );
	virtual ~igFrame( void );

	void UpdateTitle( void );

private:

	enum
	{
		ID_LoadPlugin = wxID_HIGHEST,
		ID_UnloadPlugin,
		ID_Options,
		ID_GenerateImage,
		ID_SaveImage,
		ID_Zoom25,
		ID_Zoom50,
		ID_Zoom100,
		ID_Zoom150,
		ID_Zoom200,
		ID_Exit,
		ID_About,
	};

	void OnLoadPlugin( wxCommandEvent& event );
	void OnUnloadPlugin( wxCommandEvent& event );
	void OnOptions( wxCommandEvent& event );
	void OnGenerateImage( wxCommandEvent& event );
	void OnSaveImage( wxCommandEvent& event );
	void OnZoom( wxCommandEvent& event );
	void OnExit( wxCommandEvent& event );
	void OnAbout( wxCommandEvent& event );
	void OnUpdateMenuItemUI( wxUpdateUIEvent& event );

	igCanvas* canvas;
};

// igFrame.h