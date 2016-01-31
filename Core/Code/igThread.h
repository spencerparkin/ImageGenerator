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

		typedef std::list< wxRect > RectList;
		typedef std::list< igThread* > ThreadList;

		Manager( void );
		~Manager( void );

		bool GenerateImage( int threadCount, int frameIndex = 0, int frameCount = 1, int imageAreaDivisor = 100, bool giveProgress = true );

	private:

		void StopThread( igThread* thread );

		static bool BiteOffRect( wxRect& biteRect, wxRect& bittenRect, int biteArea );

		RectList rectList;
		ThreadList threadList, lazyThreadList;
		wxCriticalSection lazyListCriticalSection;
		wxSemaphore* semaphore;
	};

	igThread( Manager* manager, wxImage* image, igPlugin::ImageGenerator* imageGenerator );
	virtual ~igThread( void );

protected:

	virtual ExitCode Entry( void ) override;

private:

	igPlugin::ImageGenerator* imageGenerator;
	Manager* manager;
	wxImage* image;
	/*volatile*/ wxRect rect;
	volatile bool stop;
};

// igThread.h