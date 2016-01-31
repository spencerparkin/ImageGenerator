// igThread.cpp

#include "igHeader.h"

//===========================================================================
igThread::igThread( Manager* manager, wxImage* image, igPlugin::ImageGenerator* imageGenerator ) : wxThread( wxTHREAD_JOINABLE )
{
	this->manager = manager;
	this->image = image;

	rect.width = 0;
	rect.height = 0;

	stop = false;

	this->imageGenerator = imageGenerator;
}

//===========================================================================
/*virtual*/ igThread::~igThread( void )
{
}

//===========================================================================
/*virtual*/ wxThread::ExitCode igThread::Entry( void )
{
	wxSize size = image->GetSize();

	while( !stop )
	{
		rect.width = 0;
		rect.height = 0;
		
		manager->lazyListCriticalSection.Enter();
		manager->lazyThreadList.push_back( this );
		manager->lazyListCriticalSection.Leave();

		// Signal the manager thread to wake up a cure our lazyness.
		manager->semaphore->Post();

		while( ( rect.width == 0 || rect.height == 0 ) && !stop )
		{
			// Spin here until we get a response from the manager thread.
			// Something tells me it would be better to use a thread-sync object for this.
		}

		if( stop )
			break;

		wxColour** imageData = new wxColour*[ rect.GetWidth() ];

		for( int i = 0; i < rect.GetWidth(); i++ )
		{
			if( stop )
				imageData[i] = nullptr;
			else
				imageData[i] = new wxColour[ rect.GetHeight() ];

			for( int j = 0; j < rect.GetHeight(); j++ )
			{
				if( stop )
					break;

				wxColour* color = &imageData[i][j];

				wxPoint point;
				point.x = rect.x + i;
				point.y = rect.y + j;

				if( !imageGenerator->GeneratePixel( point, size, *color ) )
				{
					color->Set( 0.f, 0.f, 0.f, 1.f );
				}
			}
		}

		// We might not actually need this critical section.
		wxCriticalSection* imageCriticalSection = wxGetApp().ImageCriticalSection();
		imageCriticalSection->Enter();

		for( int i = 0; i < rect.GetWidth(); i++ )
		{
			if( imageData[i] )
			{
				for( int j = 0; j < rect.GetHeight(); j++ )
				{
					wxPoint point( rect.x + i, rect.y + j );
					wxColour* color = &imageData[i][j];
					image->SetRGB( point.x, point.y, color->Red(), color->Green(), color->Blue() );
				}
			}

			delete[] imageData[i];
		}

		imageCriticalSection->Leave();

		delete[] imageData;
	}

	return 0;
}

//===========================================================================
igThread::Manager::Manager( void )
{
	semaphore = 0;
}

//===========================================================================
igThread::Manager::~Manager( void )
{
}

//===========================================================================
bool igThread::Manager::GenerateImage( int threadCount, int frameIndex /*= 0*/, int frameCount /*= 1*/,
										int imageAreaDivisor /*= 100*/, bool giveProgress /*= true*/ )
{
	wxImage* image = wxGetApp().Image();
	if( !image )
		return false;

	igPlugin* plugin = wxGetApp().Plugin();
	if( !plugin )
		return false;

	if( threadCount == 0 )
		threadCount = wxThread::GetCPUCount();

	// Subdivide the image into a bunch of subregions.
	rectList.clear();
	int biteArea = image->GetWidth() * image->GetHeight() / imageAreaDivisor;
	wxASSERT( biteArea != 0 );
	wxRect bittenRect( 0, 0, image->GetWidth(), image->GetHeight() );
	while( bittenRect.GetWidth() * bittenRect.GetHeight() > 0 )
	{
		wxRect biteRect;
		BiteOffRect( biteRect, bittenRect, biteArea );
		rectList.push_back( biteRect );
	}
	wxASSERT( bittenRect.width == 0 && bittenRect.height == 0 );

	wxProgressDialog* progressDialog = nullptr;

	if( giveProgress )
	{
		progressDialog = new wxProgressDialog(
							"Image Generation In Progress", "Generating Image...",
							rectList.size(), 0,
							wxPD_APP_MODAL | wxPD_CAN_ABORT | wxPD_ESTIMATED_TIME | wxPD_AUTO_HIDE );
	}

	semaphore = new wxSemaphore( 0, threadCount );

	// Go kick off all the worker threads.
	wxASSERT( threadList.size() == 0 );
	for( int count = 0; count < threadCount; count++ )
	{
		igPlugin::ImageGenerator* imageGenerator = plugin->NewImageGenerator();
		if( imageGenerator )
		{
			imageGenerator->frameIndex = frameIndex;
			imageGenerator->frameCount = frameCount;
			igThread* thread = new igThread( this, image, imageGenerator );
			threadList.push_back( thread );
			wxThreadError threadError = thread->Run();
			wxASSERT( threadError == wxTHREAD_NO_ERROR );
		}
	}

	// Feed the subregions to the worker threads until all parts of the image have been filled in.
	int rectCount = rectList.size();
	while( threadList.size() > 0 )
	{
		// Go to sleep until a worker thread runs out of work to do.
		semaphore->Wait();

		// Grab a lazy thread.
		lazyListCriticalSection.Enter();
		wxASSERT( lazyThreadList.size() > 0 );
		ThreadList::iterator iter = lazyThreadList.begin();
		igThread* thread = *iter;
		lazyThreadList.erase( iter );
		lazyListCriticalSection.Leave();
		
		// Give the thread some work to do or indicate that there is no more work to be done.
		if( rectList.size() == 0 )
		{
			StopThread( thread );
			thread = nullptr;
		}
		else
		{
			RectList::iterator iter = rectList.begin();
			thread->rect = *iter;
			rectList.erase( iter );
		}

		if( progressDialog )
		{
			int count = rectCount - rectList.size();
			float percentage = float( count ) / float( rectCount ) * 100.f;
			progressDialog->Update( count, wxString::Format( "Generating Image: %1.2f%%", percentage ) );
		
			if( progressDialog->WasCancelled() )
			{
				while( threadList.size() > 0 )
				{
					igThread* thread = *threadList.begin();
					StopThread( thread );
				}

				lazyThreadList.clear();
			}
		}
	}

	delete semaphore;
	semaphore = nullptr;

	delete progressDialog;

	return true;
}

//===========================================================================
void igThread::Manager::StopThread( igThread* thread )
{
	thread->stop = true;
	wxThreadError threadError = thread->Delete( 0, wxTHREAD_WAIT_BLOCK );
	wxASSERT( threadError == wxTHREAD_NO_ERROR );
	threadList.remove( thread );
	igPlugin* plugin = wxGetApp().Plugin();
	plugin->DeleteImageGenerator( thread->imageGenerator );
	delete thread;
}

//===========================================================================
/*static*/ bool igThread::Manager::BiteOffRect( wxRect& biteRect, wxRect& bittenRect, int biteArea )
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
		if( width == 0 )
			width = 1;
		bittenRect.width -= width;
		biteRect.x = bittenRect.width;
		biteRect.y = bittenRect.y;
		biteRect.width = width;
		biteRect.height = bittenRect.height;
	}
	else
	{
		int height = biteArea / bittenRect.width;
		if( height == 0 )
			height = 1;
		bittenRect.height -= height;
		biteRect.x = bittenRect.x;
		biteRect.y = bittenRect.height;
		biteRect.width = bittenRect.width;
		biteRect.height = height;
	}

	return true;
}

// igThread.cpp