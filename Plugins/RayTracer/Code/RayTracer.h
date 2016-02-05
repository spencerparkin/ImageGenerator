// RayTracer.h

//===========================================================================
class RayTracerPlugin : public igPlugin
{
	friend class ImageGenerator;
	friend class MenuEventHandler;

public:

	RayTracerPlugin( void );
	virtual ~RayTracerPlugin( void );

	virtual bool Initialize( wxMenuBar* menuBar, wxEvtHandler* updateUIHandler ) override;
	virtual bool Finalize( wxMenuBar* menuBar, wxEvtHandler* updateUIHandler ) override;

	virtual wxString Name( void ) override;

	virtual bool PreImageGeneration( wxImage* image, AnimationData* animationData ) override;
	virtual bool PostImageGeneration( wxImage* image, AnimationData* animationData ) override;

	//===========================================================================
	struct Sample
	{
		wxColour color;
		volatile bool rayCasted;
		wxCriticalSection criticalSection;
	};

	//===========================================================================
	class ImageGenerator : public igPlugin::ImageGenerator
	{
	public:
		ImageGenerator( void );
		virtual ~ImageGenerator( void );

		virtual bool GeneratePixel( const wxPoint& point, const wxSize& size, wxColour& color ) override;

		void CalculateColor( const wxPoint& point, const wxSize& size, wxColour& color );
		void CalculateSample( const wxPoint& point, const wxSize& size, Sample* sample );

		Scene* scene;
		RayTracerPlugin* plugin;
	};

	virtual igPlugin::ImageGenerator* NewImageGenerator( void ) override;
	virtual void DeleteImageGenerator( igPlugin::ImageGenerator* imageGenerator ) override;

private:

	//===========================================================================
	class MenuEventHandler : public wxEvtHandler
	{
		friend RayTracerPlugin;

	public:

		MenuEventHandler( RayTracerPlugin* rayTracerPlugin );
		virtual ~MenuEventHandler( void );

		void InsertMenu( wxMenuBar* menuBar, wxEvtHandler* updateUIHandler );
		void RemoveMenu( wxMenuBar* menuBar, wxEvtHandler* updateUIHandler );

	private:

		void OnLoadScene( wxCommandEvent& event );
		void OnUnloadScene( wxCommandEvent& event );
		void OnAntiAlias( wxCommandEvent& event );
		void OnMaxRayBounceDepth( wxCommandEvent& event );
		void OnUpdateMenuItemUI( wxUpdateUIEvent& event );

		int ID_LoadScene;
		int ID_UnloadScene;
		int ID_MaxRayBounceDepth;
		int ID_AntiAlias;
		RayTracerPlugin* rayTracerPlugin;
	};

	struct View;

	bool LoadScene( const wxString& sceneFile );
	bool UnloadScene( void );
	bool LoadView( wxXmlNode* xmlNode, View* view = nullptr );
	bool LoadViewKeys( wxXmlNode* xmlNode );
	bool LoadElement( wxXmlNode* xmlViewKeysNode );

	//===========================================================================
	struct View
	{
		c3ga::vectorE3GA eye;
		c3ga::vectorE3GA subject;
		c3ga::vectorE3GA up;
		double focalLength;
		double angle;
		double frameTime;
	};

	typedef std::list< View > ViewList;

	View theView;
	ViewList viewKeyList;
	Scene* scene;
	MenuEventHandler menuEventHandler;
	bool antiAlias;
	Sample** overSample;

	void AllocateOverSampleForImage( const wxImage* image );
	void DeallocateOverSampleForImage( const wxImage* image );
};

// RayTracer.h