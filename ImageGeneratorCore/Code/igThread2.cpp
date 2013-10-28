// igThread2.cpp

#include "igHeader.h"

//===========================================================================
igThread2::igThread2( Manager* manager, wxImage* image, igPlugin::ImageGenerator* imageGenerator ) : wxThread( wxTHREAD_JOINABLE )
{
	this->manager = manager;
	this->image = image;

	rect.width = 0;
	rect.height = 0;

	semaphore = 0;

	this->imageGenerator = imageGenerator;
}

//===========================================================================
/*virtual*/ igThread2::~igThread2( void )
{
}

//===========================================================================
/*virtual*/ wxThread::ExitCode igThread2::Entry( void )
{
	semaphore = new wxSemaphore( 0, 1 );

	wxCriticalSection* imageCriticalSection = wxGetApp().ImageCriticalSection();
	imageCriticalSection->Enter();
	wxSize size = image->GetSize();
	imageCriticalSection->Leave();

	do
	{
		// Fill in the subregion of the image that we have been given, if any.
		wxPoint point;
		for( point.x = rect.x; point.x < rect.x + rect.width; point.x++ )
		{
			for( point.y = rect.y; point.y < rect.y + rect.height; point.y++ )
			{
				wxColour color;
				if( imageGenerator->GeneratePixel( point, size, color ) )
				{
					imageCriticalSection->Enter();
					image->SetRGB( point.x, point.y, color.Red(), color.Green(), color.Blue() );
					imageCriticalSection->Leave();
				}
			}
		}

		// We're out of work.  Go ask the manager thread for more work.
		rect.width = 0;
		rect.height = 0;
		manager->semaphore->Post();

		// Wait to be signaled by the manager thread before continuing,
		// at which point, we'll either have more work to do or need to quit.
		semaphore->Wait();
		int b = 0;
	}
	while( rect.width > 0 && rect.height > 0 );

	delete semaphore;
	semaphore = 0;

	return 0;
}

//===========================================================================
igThread2::Manager::Manager( void )
{
	semaphore = 0;
}

//===========================================================================
igThread2::Manager::~Manager( void )
{
}

//===========================================================================
bool igThread2::Manager::GenerateImage( int threadCount, int imageAreaDivisor /*= 100*/ )
{
	wxImage* image = wxGetApp().Image();
	if( !image )
		return false;

	igPlugin* plugin = wxGetApp().Plugin();
	if( !plugin )
		return false;

	// Subdivide the image into a bunch of subregions.
	rectList.clear();
	int biteArea = image->GetWidth() * image->GetHeight() / imageAreaDivisor;
	wxRect bittenRect( 0, 0, image->GetWidth(), image->GetHeight() );
	while( bittenRect.GetWidth() * bittenRect.GetHeight() > 0 )
	{
		wxRect biteRect;
		BiteOffRect( biteRect, bittenRect, biteArea );
		rectList.push_back( biteRect );
	}
	wxASSERT( bittenRect.width == 0 && bittenRect.height == 0 );

	wxProgressDialog* progressDialog = new wxProgressDialog(
							"Image Generation In Progress", "Generating Image...",
							rectList.size(), 0,
							wxPD_APP_MODAL | wxPD_CAN_ABORT | wxPD_ESTIMATED_TIME | wxPD_AUTO_HIDE );

	semaphore = new wxSemaphore( 0, threadCount );

	// Go kick off all the worker threads.
	bool success = true;
	wxASSERT( threadList.size() == 0 );
	for( int count = 0; count < threadCount && success; count++ )
	{
		igPlugin::ImageGenerator* imageGenerator = plugin->NewImageGenerator();
		if( !imageGenerator )
			success = false;
		else
		{
			igThread2* thread = new igThread2( this, image, imageGenerator );
			threadList.push_back( thread );
			if( wxTHREAD_NO_ERROR != thread->Run() )
				success = false;
		}
	}

	// Feed the subregions to the worker threads until all parts of the image have been filled in.
	int rectCount = rectList.size();
	int count = 0;
	while( rectList.size() > 0 || threadList.size() > 0 )
	{
		// Go to sleep until a worker thread runs out of work to do.
		semaphore->Wait();

		// Find a thread that needs work to do.
		igThread2* thread = 0;
		for( ThreadList::iterator iter = threadList.begin(); iter != threadList.end() && !thread; iter++ )
			if( ( *iter )->rect.width == 0 && ( *iter )->rect.height == 0 )
				thread = *iter;
		wxASSERT( thread != 0 );
		
		// Give the thread some work to do or indicate that there is no more work to be done.
		wxRect rect = thread->rect;
		if( success && rectList.size() > 0 )
		{
			RectList::iterator iter = rectList.begin();
			rect = *iter;
			rectList.erase( iter );
		}
		thread->rect = rect;
		thread->semaphore->Post();

		// If we told the thread that there is no more work to be done, we can delete the thread.
		if( rect.width == 0 && rect.height == 0 )
		{
			wxThreadError threadError = thread->Delete( 0, wxTHREAD_WAIT_BLOCK );
			wxASSERT( threadError == wxTHREAD_NO_ERROR );
			threadList.remove( thread );
			plugin->DeleteImageGenerator( thread->imageGenerator );
			delete thread;
			thread = 0;
		}

		// Take this opportunity to update our progress bar.
		if( count < rectCount )
		{
			float percentage = float( ++count ) / float( rectCount ) * 100.f;
			progressDialog->Update( count, wxString::Format( "Generating Image: %1.2f%%", percentage ) );
			success = !progressDialog->WasCancelled();
		}
	}

	delete semaphore;
	semaphore;

	delete progressDialog;

	return success;
}

// igThread2.cpp