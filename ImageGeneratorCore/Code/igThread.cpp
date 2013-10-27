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

	int pixelsGenerated = 0;
	int yieldsPerThread = 10;
	int pixelsPerProgressUpdate = ( rect.width * rect.height ) / yieldsPerThread;

	wxPoint point;
	for( point.x = rect.x; point.x < rect.x + rect.width; point.x++ )
	{
		for( point.y = rect.y; point.y < rect.y + rect.height; point.y++ )
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

		// Periodically send progress updates, but at a frequency that
		// does not slow down our image generation process noticably.
		pixelsGenerated += rect.height;
		if( pixelsGenerated >= pixelsPerProgressUpdate )
		{
			Event* event = new Event( wxEVT_THREAD_PROGRESS_UPDATE, 0 );
			event->pixelsGenerated = pixelsGenerated;
			pixelsGenerated = 0;
			manager->QueueEvent( event );
			Yield();
		}
	}

	return 0;
}

//===========================================================================
const wxEventTypeTag< igThread::Event > igThread::wxEVT_THREAD_PROGRESS_UPDATE( wxNewEventType() );

//===========================================================================
igThread::Event::Event( wxEventType eventType, int id ) : wxThreadEvent( eventType, id )
{
	pixelsGenerated = 0;
}

//===========================================================================
/*virtual*/ igThread::Event::~Event( void )
{
}

//===========================================================================
/*virtual*/ wxEvent* igThread::Event::Clone( void ) const
{
	Event* event = new Event( GetEventType(), GetId() );
	event->pixelsGenerated = pixelsGenerated;
	return event;
}

//===========================================================================
igThread::Manager::Manager( void )
{
	progressDialog = 0;
	pixelsGenerated = 0;
	progressUpdateNeeded = false;

	Bind( wxEVT_THREAD_PROGRESS_UPDATE, &igThread::Manager::OnThreadProgressUpdate, this );
}

//===========================================================================
igThread::Manager::~Manager( void )
{
	wxASSERT( progressDialog == 0 );
	wxASSERT( threadList.size() == 0 );
}

//===========================================================================
void igThread::Manager::OnThreadProgressUpdate( Event& event )
{
	pixelsGenerated += event.pixelsGenerated;
	progressUpdateNeeded = true;
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

	int imageArea = image->GetWidth() * image->GetHeight();
	progressDialog = new wxProgressDialog( "Image Generation In Progress", "Generating image...",
											imageArea, 0, wxPD_APP_MODAL | wxPD_CAN_ABORT | wxPD_ESTIMATED_TIME );

	pixelsGenerated = 0;
	progressUpdateNeeded = false;

	int areaPerThread = imageArea / threadCount;
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
	bool success = true;

	igPlugin* plugin = wxGetApp().Plugin();

	while( threadList.size() > 0 )
	{
		ThreadList::iterator iter = threadList.begin();
		igThread* thread = *iter;
		
		// This loop is not as efficient as doing a "thread->Wait( wxTHREAD_WAIT_BLOCK )",
		// but I would like to get some feed-back on how long the image generation process
		// is taking.  Is there a better way to go about doing this?
		while( thread->IsRunning() )
		{
			signalTermination = progressDialog->WasCancelled();
			if( signalTermination )
				thread->Delete( 0, wxTHREAD_WAIT_BLOCK );
			else if( m_pendingEvents && !m_pendingEvents->IsEmpty() )
			{
				ProcessPendingEvents();
				if( progressUpdateNeeded )
				{
					float percentage = float( pixelsGenerated ) / float( progressDialog->GetRange() ) * 100.f;
					wxString message = wxString::Format( "Generating image: %1.2f%% (%d threads working.)", percentage, threadList.size() );
					progressDialog->Update( pixelsGenerated, message );
					progressUpdateNeeded = false;
				}
			}
		}
		
		plugin->DeleteImageGenerator( thread->imageGenerator );
		delete thread;
		threadList.erase( iter );
	}

	delete progressDialog;
	progressDialog = 0;

	if( signalTermination )
		success = false;

	return success;
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