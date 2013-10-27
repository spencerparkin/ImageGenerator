// igThread.h

//===========================================================================
// Here we implement a pre-farmed method of distributing the work-load of
// generating a single image to multiple threads.  A, perhaps, better solution
// to the problem would be an on-demand farming method where each thread is
// given work at the rate at which it is able to complete the work it's given.
class igThread : public wxThread
{
	friend class Manager;

public:

	//===========================================================================
	class Event : public wxThreadEvent
	{
	public:

		Event( wxEventType eventType, int id );
		virtual ~Event( void );

		virtual wxEvent* Clone( void ) const override;

		int pixelsGenerated;
	};

	//===========================================================================
	static const wxEventTypeTag< Event > wxEVT_THREAD_PROGRESS_UPDATE;

	//===========================================================================
	class Manager : public wxEvtHandler
	{
	public:

		Manager( void );
		~Manager( void );

		bool KickOffThreads( int threadCount );
		bool WaitForThreads( bool signalTermination = false );

	private:

		void OnThreadProgressUpdate( Event& event );

		static bool BiteOffRect( wxRect& biteRect, wxRect& bittenRect, int biteArea );

		typedef std::list< igThread* > ThreadList;
		ThreadList threadList;
		wxProgressDialog* progressDialog;
		int pixelsGenerated;
		bool progressUpdateNeeded;
	};

	igThread( Manager* manager );
	virtual ~igThread( void );

	// The thread is a shared resource between itself and the main
	// thread, but I'm not always certain when I need to lock this
	// critical section while using the thread in either context.
	wxCriticalSection criticalSection;

protected:

	virtual ExitCode Entry( void ) override;

private:

	Manager* manager;
	igPlugin::ImageGenerator* imageGenerator;
	wxRect rect;
};

// igThread.h