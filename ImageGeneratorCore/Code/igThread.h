// igThread.h

//===========================================================================
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