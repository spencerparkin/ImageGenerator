// igCanvas.cpp

#include "igHeader.h"

//===========================================================================
igCanvas::igCanvas( wxWindow* parent ) : wxScrolledCanvas( parent )
{
	zoom = ZOOM_100;
	cachedBitmap = 0;
	rubberBanding = false;
	newRubberBand.x = oldRubberBand.x = 0;
	newRubberBand.y = oldRubberBand.y = 0;
	newRubberBand.width = oldRubberBand.width = 0;
	newRubberBand.height = oldRubberBand.height = 0;

	Bind( wxEVT_LEFT_DOWN, &igCanvas::OnMouseLeftDown, this );
	Bind( wxEVT_LEFT_UP, &igCanvas::OnMouseLeftUp, this );
	Bind( wxEVT_MOTION, &igCanvas::OnMouseMotion, this );
}

//===========================================================================
/*virtual*/ igCanvas::~igCanvas( void )
{
	InvalidateCachedBitmap();
}

//===========================================================================
void igCanvas::SetZoom( Zoom zoom )
{
	this->zoom = zoom;
}

//===========================================================================
igCanvas::Zoom igCanvas::GetZoom( void )
{
	return zoom;
}

//===========================================================================
void igCanvas::InvalidateCachedBitmap( void )
{
	if( cachedBitmap )
	{
		delete cachedBitmap;
		cachedBitmap = 0;
	}
}

//===========================================================================
wxPoint igCanvas::MouseToCanvas( const wxPoint& mousePos )
{
	wxPoint viewStart = GetViewStart();
	return mousePos + viewStart;
}

//===========================================================================
void igCanvas::OnMouseLeftDown( wxMouseEvent& event )
{
	rubberBanding = true;

	oldRubberBand.x = 0;
	oldRubberBand.y = 0;
	oldRubberBand.width = 0;
	oldRubberBand.height = 0;

	newRubberBand.SetLeftTop( MouseToCanvas( event.GetPosition() ) );
	newRubberBand.SetWidth(0);
	newRubberBand.SetHeight(0);

	Refresh( false );
}

//===========================================================================
void igCanvas::OnMouseLeftUp( wxMouseEvent& event )
{
	rubberBanding = false;
	
	wxImage* image = wxGetApp().Image();
	igPlugin* plugin = wxGetApp().Plugin();
	if( image && plugin && plugin->SubregionSelect( newRubberBand, image->GetSize() ) )
		if( !wxGetApp().GenerateImage() )
			wxMessageBox( "Failed to generate image." );
		else
			InvalidateCachedBitmap();
	
	Refresh();
}

//===========================================================================
void igCanvas::OnMouseMotion( wxMouseEvent& event )
{
	if( rubberBanding )
	{
		newRubberBand.SetBottomRight( MouseToCanvas( event.GetPosition() ) );
		Refresh( false );
	}
}

//===========================================================================
/*virtual*/ void igCanvas::OnDraw( wxDC& dc )
{
	if( rubberBanding )
	{
		wxRasterOperationMode rasterOperationMode = dc.GetLogicalFunction();
		dc.SetLogicalFunction( wxXOR );

		dc.SetPen( *wxWHITE_PEN );
		dc.SetBrush( *wxBLACK_BRUSH );

		dc.DrawRectangle( oldRubberBand );
		dc.DrawRectangle( newRubberBand );

		oldRubberBand = newRubberBand;

		dc.SetLogicalFunction( rasterOperationMode );
	}
	else if( GenerateCachedBitmap() )
	{
		// TODO: Can we increase performance by only drawing the part of our
		//       cached bitmap that overlaps the region of the window that
		//       needs to be redrawn?
		dc.DrawBitmap( *cachedBitmap, 0, 0 );
	}
}

//===========================================================================
bool igCanvas::GenerateCachedBitmap( void )
{
	if( !cachedBitmap )
	{
		wxGetApp().ImageCriticalSection()->Enter();
		wxImage* image = wxGetApp().Image();
		if( image )
		{
			float zoomFactor = 1.f;
			switch( zoom )
			{
				case ZOOM_25: zoomFactor = 0.25f; break;
				case ZOOM_50: zoomFactor = 0.5f; break;
				case ZOOM_100: zoomFactor = 1.f; break;
				case ZOOM_150: zoomFactor = 1.5f; break;
				case ZOOM_200: zoomFactor = 2.f; break;
			}

			wxBusyCursor busyCursor;
			wxImage scaledImage = image->Copy();
			wxSize size = image->GetSize();
			size.SetWidth( zoomFactor * float( size.GetWidth() ) );
			size.SetHeight( zoomFactor * float( size.GetHeight() ) );
			scaledImage.Rescale( size.GetWidth(), size.GetHeight(), wxIMAGE_QUALITY_HIGH );
			cachedBitmap = new wxBitmap( scaledImage );
			SetVirtualSize( size );
		}
		wxGetApp().ImageCriticalSection()->Leave();
	}

	return cachedBitmap ? true : false;
}

// igCanvas.cpp