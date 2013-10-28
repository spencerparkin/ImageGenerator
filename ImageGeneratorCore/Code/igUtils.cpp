// igUtils.cpp

#include "igHeader.h"

//===========================================================================
bool BiteOffRect( wxRect& biteRect, wxRect& bittenRect, int biteArea )
{
	if( biteArea <= 0 )
		return false;

	int area = bittenRect.width * bittenRect.height;
	if( !area )
		return false;

	if( abs( area - biteArea ) < biteArea / 2 )
	{
		biteRect = bittenRect;
		bittenRect.width = 0;
		bittenRect.height = 0;
		return true;
	}
	
	if( bittenRect.width > bittenRect.height )
	{
		int width = biteArea / bittenRect.height;
		bittenRect.width -= width;
		biteRect.x = bittenRect.width;
		biteRect.y = bittenRect.y;
		biteRect.width = width;
		biteRect.height = bittenRect.height;
	}
	else
	{
		int height = biteArea / bittenRect.width;
		bittenRect.height -= height;
		biteRect.x = bittenRect.x;
		biteRect.y = bittenRect.height;
		biteRect.width = bittenRect.width;
		biteRect.height = height;
	}

	return true;
}

// igUtils.cpp