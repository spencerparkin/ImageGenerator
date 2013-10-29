// RayTracer.h

//===========================================================================
class RayTracerPlugin : public igPlugin
{
	friend class ImageGenerator;

public:

	RayTracerPlugin( void );
	virtual ~RayTracerPlugin( void );

	virtual bool Initialize( void ) override;
	virtual bool Finalize( void ) override;

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
};

// RayTracer.h