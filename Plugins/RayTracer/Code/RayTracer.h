// RayTracer.h

//===========================================================================
class RayTracerPlugin : public igPlugin
{
	friend class ImageGenerator;
	friend class MenuEventHandler;

public:

	RayTracerPlugin( void );
	virtual ~RayTracerPlugin( void );

	virtual bool Initialize( wxMenuBar* menuBar ) override;
	virtual bool Finalize( wxMenuBar* menuBar ) override;

	virtual wxString Name( void ) override;

	virtual bool PreImageGeneration( wxImage* image ) override;
	virtual bool PostImageGeneration( wxImage* image ) override;

	//===========================================================================
	class ImageGenerator : public igPlugin::ImageGenerator
	{
	public:
		ImageGenerator( void );
		virtual ~ImageGenerator( void );

		virtual bool GeneratePixel( const wxPoint& point, const wxSize& size, wxColour& color ) override;

		Scene* scene;
		RayTracerPlugin* plugin;
	};

	virtual igPlugin::ImageGenerator* NewImageGenerator( void ) override;
	virtual void DeleteImageGenerator( igPlugin::ImageGenerator* imageGenerator ) override;

private:

	//===========================================================================
	class MenuEventHandler : public wxEvtHandler
	{
	public:

		MenuEventHandler( RayTracerPlugin* rayTracerPlugin );
		virtual ~MenuEventHandler( void );

		void InsertMenu( wxMenuBar* menuBar );
		void RemoveMenu( wxMenuBar* menuBar );

	private:

		void OnLoadScene( wxCommandEvent& event );
		void OnUnloadScene( wxCommandEvent& event );

		void OnUpdateMenuItemUI( wxUpdateUIEvent& event );

		int ID_LoadScene;
		int ID_UnloadScene;
		RayTracerPlugin* rayTracerPlugin;
	};

	bool LoadScene( const wxString& sceneFile );
	bool UnloadScene( void );
	bool LoadView( wxXmlNode* xmlNode );
	bool LoadElement( wxXmlNode* xmlNode );

	//===========================================================================
	struct View
	{
		c3ga::vectorE3GA eye;
		c3ga::vectorE3GA direction;
		c3ga::vectorE3GA up;
		double focalLength;
		double angle;
	};

	View view;
	Scene* scene;
	MenuEventHandler menuEventHandler;
};

// RayTracer.h