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

	bool GenerateCachedBitmap( void );

	wxBitmap* cachedBitmap;
	Zoom zoom;
};

// igCanvas.h