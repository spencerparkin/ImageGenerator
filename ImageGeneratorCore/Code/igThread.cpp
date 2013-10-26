// igThread.cpp

#include "igHeader.h"

//===========================================================================
igThread::igThread( Manager* manager ) : wxThread( wxTHREAD_JOINABLE )
{
	this->manager = manager;
	imageGenerator = 0;
	rect.x = rect.width = 0;
	rect.y = rect.height = 0;
}

//===========================================================================
/*virtual*/ igThread::~igThread( void )
{
}

//===========================================================================
/*virtual*/ igThread::ExitCode igThread::Entry( void )
{
	wxCriticalSection* imageCriticalSection = wxGetApp().ImageCriticalSection();
	imageCriticalSection->Enter();
	wxImage* image = wxGetApp().Image();
	wxSize size = image->GetSize();
	imageCriticalSection->Leave();

	wxPoint point;
	for( point.x = rect.x; point.x < rect.x + rect.width; point.x++ )
	{
		for( point.y = rect.y; point.y < rect.y + rect.width; point.y++ )
		{
			if( TestDestroy() )
				return 0;

			wxColour color;
			if( !imageGenerator->GeneratePixel( point, size, color ) )
				return 0;

			// A performance increase could probably be obtained by realizing that
			// multiple threads can populate the same pixel buffer without the need
			// to enter/leave a critical section at all, but for now, since I'm going
			// through the wxImage class, I have to enter/leave such a section to be
			// safe, because I can't guarentee the thread-safety of the wxImage class.
			// In any case, since we'll be spending most of our time formulating the
			// pixel instead of writing it, maybe it doesn't matter that much.
			imageCriticalSection->Enter();
			image->SetRGB( point.x, point.y, color.Red(), color.Green(), color.Blue() );
			imageCriticalSection->Leave();
		}
	}

	return 0;
}

//===========================================================================
igThread::Event::Event( wxEventType eventType, int id ) : wxThreadEvent( eventType, id )
{
}

//===========================================================================
/*virtual*/ igThread::Event::~Event( void )
{
}

//===========================================================================
/*virtual*/ wxEvent* igThread::Event::Clone( void ) const
{
	Event* event = new Event( GetEventType(), GetId() );
	//...
	return event;
}

//===========================================================================
igThread::Manager::Manager( void )
{
}

//===========================================================================
igThread::Manager::~Manager( void )
{
	wxASSERT( threadList.size() == 0 );
}

//===========================================================================
bool igThread::Manager::KickOffThreads( int threadCount )
{
	igPlugin* plugin = wxGetApp().Plugin();
	if( !plugin )
		return false;

	wxImage* image = wxGetApp().Image();
	if( !image )
		return false;

	int areaPerThread = ( image->GetWidth() * image->GetHeight() ) / threadCount;
	wxRect rect( 0, 0, image->GetWidth(), image->GetHeight() );

	bool success = true;

	int count;
	for( count = 0; count < threadCount; count++ )
	{
		wxRect biteRect;
		if( !BiteOffRect( biteRect, rect, areaPerThread ) )
			break;

		igPlugin::ImageGenerator* imageGenerator = plugin->NewImageGenerator();
		if( !imageGenerator )
			break;

		igThread* thread = new igThread( this );
		thread->imageGenerator = imageGenerator;
		thread->rect = biteRect;
		threadList.push_back( thread );

		if( wxTHREAD_NO_ERROR != thread->Run() )
			break;
	}

	if( count < threadCount )
		success = false;
	else
		wxASSERT( rect.width == 0 && rect.height == 0 );

	if( !success )
		WaitForThreads( true );

	return success;
}

//===========================================================================
bool igThread::Manager::WaitForThreads( bool signalTermination /*= false*/ )
{
	igPlugin* plugin = wxGetApp().Plugin();

	while( threadList.size() > 0 )
	{
		ThreadList::iterator iter = threadList.begin();
		igThread* thread = *iter;
		
		if( signalTermination )
			thread->Delete( 0, wxTHREAD_WAIT_BLOCK );
		else
			thread->Wait( wxTHREAD_WAIT_BLOCK );
		
		// TODO: This appears to work, but when I exit the application, I get an access violation.  Why?
		plugin->DeleteImageGenerator( thread->imageGenerator );
		delete thread;
		threadList.erase( iter );
	}

	return true;
}

//===========================================================================
/*static*/ bool igThread::Manager::BiteOffRect( wxRect& biteRect, wxRect& bittenRect, int biteArea )
{
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

// igThread.cpp