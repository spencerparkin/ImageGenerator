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
	wxCriticalSection* ImageCriticalSection( void );

	bool LoadPlugin( const wxString& pluginPath );
	bool UnloadPlugin( void );

	bool GenerateImage( void );
	bool DeleteImage( void );

	enum { COLOR_COMPONENTS_PER_PIXEL = 3 };

private:

	HMODULE pluginHandle;
	igPlugin* plugin;
	
	unsigned char* imageData;
	wxSize imageSize;
	wxImage* image;
	wxCriticalSection imageCriticalSection;

	int threadCount;
};

//===========================================================================
wxDECLARE_APP( igApp );

// igApp.h