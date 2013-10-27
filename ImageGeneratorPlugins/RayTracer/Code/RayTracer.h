// RayTracer.h

#include <wx/setup.h>
#include <wx/colour.h>
#include <wx/gdicmn.h>
#include <wx/image.h>
#include <igPlugin.h>

//===========================================================================
class RayTracerPlugin : public igPlugin
{
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
	};

	virtual igPlugin::ImageGenerator* NewImageGenerator( void ) override;
	virtual void DeleteImageGenerator( igPlugin::ImageGenerator* imageGenerator ) override;

private:
};

// RayTracer.h