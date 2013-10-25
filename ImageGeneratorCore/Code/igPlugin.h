// igPlugin.h

//===========================================================================
// Image generator plugins implement a derivative of this class and a
// function named "NewImageGeneratorPlugin" that returns a new instance
// of the derived class.  Similarly, a function named "DeleteImageGeneratorPlugin"
// should be provided that deletes such an instance.  The plugin DLL will
// need wxWidgets as a linker dependency.
class igPlugin
{
public:

	igPlugin( void ) {}
	virtual ~igPlugin( void ) {}

	typedef igPlugin* ( *NewImageGeneratorPluginFunc )( void );
	typedef void ( *DeleteImageGeneratorPluginFunc )( igPlugin* );

	virtual bool Initialize( void ) = 0;
	virtual bool Finalize( void ) = 0;

	virtual bool PreImageGeneration( wxImage* image ) { return true; };
	virtual bool PostImageGeneration( wxImage* image ) { return true; };

	//===========================================================================
	// The image generator framework will create an instance of this class
	// per thread.  Individual worker threads will call upon such instances
	// to generate the pixels of the image.  It is up to the user to insure
	// that the class is thread safe.
	class ImageGenerator
	{
	public:

		virtual bool GeneratePixel( const wxPoint& point, const wxSize& size, wxColour& color ) = 0;
	};

	virtual ImageGenerator* NewImageGenerator( void ) = 0;
	virtual void DeleteImageGenerator( ImageGenerator* imageGenerator ) = 0;
};

// igPlugin.h