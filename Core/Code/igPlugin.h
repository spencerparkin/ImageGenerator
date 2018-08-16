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

	virtual bool Initialize( wxMenuBar* menuBar, wxEvtHandler* updateUIHandler ) = 0;
	virtual bool Finalize( wxMenuBar* menuBar, wxEvtHandler* updateUIHandler ) = 0;

	virtual wxString Name( void ) = 0;

	struct AnimationData
	{
		bool animating;
		int frameIndex;
		int frameCount;
		float frameRate;
	};

	virtual bool PreImageGeneration( wxImage* image, AnimationData* animationData ) { return true; };
	virtual bool PostImageGeneration( wxImage* image, AnimationData* animationData ) { return true; };

	virtual bool SubregionSelect( const wxRect& rect, const wxSize& size ) { return false; }

	//===========================================================================
	// The image generator framework will create an instance of this class
	// per thread.  Individual worker threads will call upon such instances
	// to generate the pixels of the image.  It is up to the user to insure
	// that the class is thread safe.
	class ImageGenerator
	{
	public:
		ImageGenerator( void ) { animationData = nullptr; }
		virtual ~ImageGenerator( void ) {}

		virtual bool GeneratePixel( const wxPoint& point, const wxSize& size, wxColour& color ) = 0;

		AnimationData* animationData;
	};

	virtual ImageGenerator* NewImageGenerator( void ) = 0;
	virtual void DeleteImageGenerator( ImageGenerator* imageGenerator ) = 0;

	static int CalcMaxMenuId( wxMenuBar* menuBar )
	{
		int maxId = 0;
		for( unsigned int i = 0; i < menuBar->GetMenuCount(); i++ )
		{
			const wxMenu* menu = menuBar->GetMenu( i );
			const wxMenuItemList& menuItemList = menu->GetMenuItems();
			for( wxMenuItemList::const_iterator iter = menuItemList.begin(); iter != menuItemList.end(); iter++ )
			{
				const wxMenuItem* menuItem = *iter;
				if( maxId < menuItem->GetId() )
					maxId = menuItem->GetId();
			}
		}

		return maxId;
	}

	wxString path;
};

// igPlugin.h