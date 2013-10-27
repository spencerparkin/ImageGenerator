// igCanvas.cpp

#include "igHeader.h"

//===========================================================================
igCanvas::igCanvas( wxWindow* parent ) : wxScrolledCanvas( parent )
{
	zoom = ZOOM_100;
	cachedBitmap = 0;
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
/*virtual*/ void igCanvas::OnDraw( wxDC& dc )
{
	if( GenerateCachedBitmap() )
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
	}

	return cachedBitmap ? true : false;
}

// igCanvas.cpp