// Fractal.h

#include <wx/setup.h>
#include <wx/colour.h>
#include <wx/gdicmn.h>
#include <wx/image.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <igPlugin.h>
#include <boost/multiprecision/cpp_dec_float.hpp>

typedef long double scalar_t;
//typedef boost::multiprecision::cpp_dec_float_50 scalar_t;

//===========================================================================
class FractalPlugin : public igPlugin
{
	friend class ImageGenerator;

public:

	FractalPlugin( void );
	virtual ~FractalPlugin( void );

	virtual bool Initialize( wxMenuBar* menuBar, wxEvtHandler* updateUIHandler ) override;
	virtual bool Finalize( wxMenuBar* menuBar, wxEvtHandler* updateUIHandler ) override;

	virtual wxString Name( void ) override;

	virtual bool PreImageGeneration( wxImage* image, AnimationData* animationData ) override;
	virtual bool PostImageGeneration( wxImage* image, AnimationData* animationData ) override;

	virtual bool SubregionSelect( const wxRect& rect, const wxSize& size ) override;

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

	//===========================================================================
	class MenuEventHandler : public wxEvtHandler
	{
	public:

		MenuEventHandler( FractalPlugin* fractalPlugin );
		virtual ~MenuEventHandler( void );

		void InsertMenu( wxMenuBar* menuBar, wxEvtHandler* updateUIHandler );
		void RemoveMenu( wxMenuBar* menuBar, wxEvtHandler* updateUIHandler );

	private:

		void OnResetRegion( wxCommandEvent& event );
		void OnSetTargetRegion( wxCommandEvent& event );
		void OnUpdateMenuItemUI( wxUpdateUIEvent& event );

		int ID_ResetRegion;
		int ID_SetTargetRegion;
		FractalPlugin* fractalPlugin;
	};

	void GenerateColorTable( void );

	scalar_t Zoom( scalar_t source, scalar_t target, scalar_t zoomRate );

	void ResetRegion( void );
	void SetTargetRegion( void );

	wxColour* colorTable;
	int colorTableSize;

	struct Region
	{
		scalar_t realMin, realMax;
		scalar_t imagMin, imagMax;
	};

	Region region, regionTarget, regionSource;

	int maxIters;

	MenuEventHandler menuEventHandler;
};

// Fractal.h