// igThread2.h

//===========================================================================
class igThread2 : public wxThread
{
	friend class Manager;

public:

	//===========================================================================
	class Manager
	{
		friend igThread2;

	public:

		Manager( void );
		~Manager( void );

		bool GenerateImage( int threadCount, int imageAreaDivisor = 100 );

	private:

		wxSemaphore* semaphore;

		typedef std::list< wxRect > RectList;
		RectList rectList;

		typedef std::list< igThread2* > ThreadList;
		ThreadList threadList;
	};

	igThread2( Manager* manager, wxImage* image, igPlugin::ImageGenerator* imageGenerator );
	virtual ~igThread2( void );

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

// igThread2.h