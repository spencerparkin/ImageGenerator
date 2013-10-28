// igCanvas.h

//===========================================================================
class igCanvas : public wxScrolledCanvas
{
public:

	igCanvas( wxWindow* parent );
	virtual ~igCanvas( void );

	virtual void OnDraw( wxDC& dc ) override;

	enum Zoom
	{
		ZOOM_25,
		ZOOM_50,
		ZOOM_100,
		ZOOM_150,
		ZOOM_200,
	};

	void SetZoom( Zoom zoom );
	Zoom GetZoom( void );

	void InvalidateCachedBitmap( void );

private:

	void OnMouseLeftDown( wxMouseEvent& event );
	void OnMouseLeftUp( wxMouseEvent& event );
	void OnMouseMotion( wxMouseEvent& event );

	bool GenerateCachedBitmap( void );

	wxPoint MouseToCanvas( const wxPoint& mousePos );

	wxBitmap* cachedBitmap;
	Zoom zoom;
	bool rubberBanding;
	wxRect newRubberBand;
	wxRect oldRubberBand;
};

// igCanvas.h