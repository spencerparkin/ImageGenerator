// igFrame.h

//===========================================================================
class igFrame : public wxFrame
{
public:

	igFrame( void );
	virtual ~igFrame( void );

private:

	enum
	{
		ID_LoadPlugin = wxID_HIGHEST,
		ID_UnloadPlugin,
		ID_GenerateImage,
		ID_SaveImage,
		ID_Exit,
		ID_About,
	};

	void OnLoadPlugin( wxCommandEvent& event );
	void OnUnloadPlugin( wxCommandEvent& event );
	void OnGenerateImage( wxCommandEvent& event );
	void OnSaveImage( wxCommandEvent& event );
	void OnExit( wxCommandEvent& event );
	void OnAbout( wxCommandEvent& event );
	void OnUpdateMenuItemUI( wxUpdateUIEvent& event );
};

// igFrame.h