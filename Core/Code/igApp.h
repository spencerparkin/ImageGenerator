// igApp.h

//===========================================================================
class igApp : public wxApp
{
public:

	igApp( void );
	virtual ~igApp( void );

	virtual bool OnInit( void ) override;
	virtual int OnExit( void ) override;

	virtual void OnInitCmdLine( wxCmdLineParser& parser ) override;
	virtual bool OnCmdLineParsed( wxCmdLineParser& parser ) override;

	igPlugin* Plugin( void );
	wxImage* Image( void );
	wxCriticalSection* ImageCriticalSection( void );

	bool LoadPlugin( const wxString& pluginPath );
	bool UnloadPlugin( void );

	bool GenerateImage( void );
	bool DeleteImage( void );

	enum { COMPONENTS_PER_PIXEL = 3 };

	struct Options
	{
		int threadCount;
		wxSize imageSize;
	};

	const Options& GetOptions( void );
	void SetOptions( const Options& options );

private:

	HMODULE pluginHandle;
	igPlugin* plugin;
	
	unsigned char* imageData;
	wxImage* image;
	wxCriticalSection imageCriticalSection;

	Options options;
};

//===========================================================================
wxDECLARE_APP( igApp );

// igApp.h