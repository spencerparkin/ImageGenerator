// igApp.h

//===========================================================================
class igApp : public wxApp
{
public:

	igApp( void );
	virtual ~igApp( void );

	virtual bool OnInit( void ) override;
	virtual int OnExit( void ) override;

	igPlugin* Plugin( void );
	wxImage* Image( void );

	bool LoadPlugin( const wxString& pluginPath );
	bool UnloadPlugin( void );

	bool GenerateImage( void );
	bool DeleteImage( void );

private:

	HMODULE pluginHandle;
	igPlugin* plugin;
	unsigned char* imageData;
	wxSize imageSize;
	wxImage* image;
	int threadCount;
};

//===========================================================================
wxDECLARE_APP( igApp );

// igApp.h