// igThread.h

//===========================================================================
class igThread : public wxThread
{
	friend class Manager;

public:

	//===========================================================================
	class Manager
	{
		friend igThread;

	public:

		Manager( void );
		~Manager( void );

		bool GenerateImage( int threadCount, int imageAreaDivisor = 100 );

	private:

		static bool BiteOffRect( wxRect& biteRect, wxRect& bittenRect, int biteArea );

		wxSemaphore* semaphore;

		typedef std::list< wxRect > RectList;
		RectList rectList;

		typedef std::list< igThread* > ThreadList;
		ThreadList threadList;
	};

	igThread( Manager* manager, wxImage* image, igPlugin::ImageGenerator* imageGenerator );
	virtual ~igThread( void );

	// The thread is a shared resource between itself and the main
	// thread, but I'm not always certain when I need to lock this
	// critical section while using the thread in either context.
	//wxCriticalSection criticalSection;

protected:

	virtual ExitCode Entry( void ) override;

private:

	igPlugin::ImageGenerator* imageGenerator;
	wxSemaphore* semaphore;
	Manager* manager;
	wxImage* image;
	/*volatile*/ wxRect rect;
};

// igThread.h