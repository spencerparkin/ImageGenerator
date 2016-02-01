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
	bool UnloadPlugin( bool frameDeleted = false );

	bool GenerateImage( int frameIndex = 0, bool giveProgress = true );
	bool DeleteImage( void );

	bool GenerateVideo( const wxString& videoPath );

	enum { COMPONENTS_PER_PIXEL = 3 };

	struct Options
	{
		int threadCount;
		int frameCount;
		int frameRate;
		wxSize imageSize;
	};

	struct FrameLayout
	{
		wxPoint pos;
		wxSize size;
	};

	FrameLayout frameLayout;

	const Options& GetOptions( void );
	void SetOptions( const Options& options );

private:

	void SaveConfiguration( void );
	void RestoreConfiguration( void );

	wxString GetFFMpegError( int ret );
	bool StuffImageInFrame( AVFrame* frame, int pix_fmt );
	bool GetSample( int i, int j, float& r, float& g, float& b );
	void ConvertSampleRGBtoYUV( float r, float g, float b, float& y, float& u, float& v );

	HMODULE pluginHandle;
	igPlugin* plugin;
	
	unsigned char* imageData;
	wxImage* image;
	wxCriticalSection imageCriticalSection;

	Options options;

	igFrame* frame;
};

//===========================================================================
wxDECLARE_APP( igApp );

// igApp.h