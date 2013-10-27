// Fractal.h

#include <wx/setup.h>
#include <wx/colour.h>
#include <wx/gdicmn.h>
#include <wx/image.h>
#include <igPlugin.h>

//===========================================================================
class FractalPlugin : public igPlugin
{
	friend class ImageGenerator;

public:

	FractalPlugin( void );
	virtual ~FractalPlugin( void );

	virtual bool Initialize( void ) override;
	virtual bool Finalize( void ) override;

	virtual wxString Name( void ) override;

	virtual bool PreImageGeneration( wxImage* image ) override;
	virtual bool PostImageGeneration( wxImage* image ) override;

	//===========================================================================
	class ImageGenerator : public igPlugin::ImageGenerator
	{
	public:
		ImageGenerator( FractalPlugin* fractalPlugin );
		virtual ~ImageGenerator( void );

		virtual bool GeneratePixel( const wxPoint& point, const wxSize& size, wxColour& color ) override;

		FractalPlugin* fractalPlugin;
	};

	virtual igPlugin::ImageGenerator* NewImageGenerator( void ) override;
	virtual void DeleteImageGenerator( igPlugin::ImageGenerator* imageGenerator ) override;

private:

	void GenerateColorTable( void );

	wxColour* colorTable;
	int colorTableSize;

	double realMin, realMax;
	double imagMin, imagMax;

	int maxIters;
};

// Fractal.h