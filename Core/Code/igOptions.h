// igOptions.h

//===========================================================================
class igOptionsDialog : public wxDialog
{
public:

	igOptionsDialog( wxWindow* parent, const igApp::Options& options );
	virtual ~igOptionsDialog( void );

	const igApp::Options& Options( void );

private:

	igApp::Options options;
};

// igOptions.h