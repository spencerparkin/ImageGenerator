// igOptions.cpp

#include "igHeader.h"

//===========================================================================
igOptionsDialog::igOptionsDialog( wxWindow* parent, const igApp::Options& options ) :
				wxDialog( parent, wxID_ANY, "Options", wxDefaultPosition, wxSize( 250, 350 /*-1*/ ) )
{
	this->options = options;

	wxIntegerValidator< int > imageWidthValidator( &this->options.imageSize.x );
	wxIntegerValidator< int > imageHeightValidator( &this->options.imageSize.y );
	wxIntegerValidator< int > threadCountValidator( &this->options.threadCount );
	wxIntegerValidator< int > frameCountValidator( &this->options.frameCount );
	wxIntegerValidator< int > frameRateValidator( &this->options.frameRate );

	imageWidthValidator.SetMin( 1 );
	imageHeightValidator.SetMin( 1 );

	threadCountValidator.SetMin( 0 );
	threadCountValidator.SetMax( 32 );

	frameCountValidator.SetMin( 0 );
	frameCountValidator.SetMax( 1024 );

	frameRateValidator.SetMin( 1 );
	frameRateValidator.SetMax( 60 );

	wxTextCtrl* imageWidthText = new wxTextCtrl( this, wxID_ANY, wxEmptyString,
										wxDefaultPosition, wxDefaultSize, 0,
										imageWidthValidator );
	wxTextCtrl* imageHeightText = new wxTextCtrl( this, wxID_ANY, wxEmptyString,
										wxDefaultPosition, wxDefaultSize, 0,
										imageHeightValidator );
	wxTextCtrl* threadCountText = new wxTextCtrl( this, wxID_ANY, wxEmptyString,
										wxDefaultPosition, wxDefaultSize, 0,
										threadCountValidator );
	wxTextCtrl* frameCountText = new wxTextCtrl( this, wxID_ANY, wxEmptyString,
										wxDefaultPosition, wxDefaultSize, 0,
										frameCountValidator );
	wxTextCtrl* frameRateText = new wxTextCtrl( this, wxID_ANY, wxEmptyString,
										wxDefaultPosition, wxDefaultSize, 0,
										frameRateValidator );

	wxStaticText* imageWidthLabel = new wxStaticText( this, wxID_ANY, "Width:",
										wxDefaultPosition, wxSize( 50, -1 ),
										wxALIGN_RIGHT );
	wxStaticText* imageHeightLabel = new wxStaticText( this, wxID_ANY, "Height:",
										wxDefaultPosition, wxSize( 50, -1 ),
										wxALIGN_RIGHT );
	wxStaticText* threadCountLabel = new wxStaticText( this, wxID_ANY, "Threads:",
										wxDefaultPosition, wxSize( 50, -1 ),
										wxALIGN_RIGHT );
	wxStaticText* frameCountLabel = new wxStaticText( this, wxID_ANY, "Frames:",
										wxDefaultPosition, wxSize( 50, -1 ),
										wxALIGN_RIGHT );
	wxStaticText* frameRateLabel = new wxStaticText( this, wxID_ANY, "FPS:",
										wxDefaultPosition, wxSize( 50, -1 ),
										wxALIGN_RIGHT );

	wxBoxSizer* widthBoxSizer = new wxBoxSizer( wxHORIZONTAL );
	wxBoxSizer* heightBoxSizer = new wxBoxSizer( wxHORIZONTAL );

	widthBoxSizer->Add( imageWidthLabel, 0, wxLEFT, 5 );
	widthBoxSizer->Add( imageWidthText, 1, wxEXPAND );
	heightBoxSizer->Add( imageHeightLabel, 0, wxLEFT, 5 );
	heightBoxSizer->Add( imageHeightText, 1, wxEXPAND );

	wxStaticBoxSizer* imageStaticBox = new wxStaticBoxSizer( wxVERTICAL, this, "Image" );
	imageStaticBox->Add( widthBoxSizer );
	imageStaticBox->Add( heightBoxSizer, 0, wxTOP, 5 );

	wxBoxSizer* threadCountBoxSizer = new wxBoxSizer( wxHORIZONTAL );
	threadCountBoxSizer->Add( threadCountLabel, 0, wxLEFT, 5 );
	threadCountBoxSizer->Add( threadCountText, 1, wxEXPAND );

	wxStaticBoxSizer* threadStaticBox = new wxStaticBoxSizer( wxVERTICAL, this, "Thread" );
	threadStaticBox->Add( threadCountBoxSizer );

	wxBoxSizer* frameCountSizer = new wxBoxSizer( wxHORIZONTAL );
	wxBoxSizer* frameRateSizer = new wxBoxSizer( wxHORIZONTAL );

	frameCountSizer->Add( frameCountLabel, 0, wxLEFT, 5 );
	frameCountSizer->Add( frameCountText, 1, wxEXPAND );
	frameRateSizer->Add( frameRateLabel, 0, wxLEFT, 5 );
	frameRateSizer->Add( frameRateText, 1, wxEXPAND );

	wxStaticBoxSizer* videoStaticBox = new wxStaticBoxSizer( wxVERTICAL, this, "Video" );
	videoStaticBox->Add( frameCountSizer );
	videoStaticBox->Add( frameRateSizer, 0, wxTOP, 5 );

	wxBoxSizer* boxSizer = new wxBoxSizer( wxVERTICAL );
	boxSizer->Add( imageStaticBox, 1, wxEXPAND | wxALL, 10 );
	boxSizer->Add( threadStaticBox, 1, wxEXPAND | wxLEFT | wxRIGHT, 10 );
	boxSizer->Add( videoStaticBox, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10 );

#ifndef VIDEO_SUPPORT
	frameCountText->Enable(false);
	frameRateText->Enable(false);
#endif //!VIDEO_SUPPORT

	wxSizer* sizer = CreateSeparatedButtonSizer( wxOK | wxCANCEL );
	if( sizer )
		boxSizer->Add( sizer, 1, wxEXPAND | wxALL, 10 );
	
	SetSizer( boxSizer );
}

//===========================================================================
/*virtual*/ igOptionsDialog::~igOptionsDialog( void )
{
}

//===========================================================================
const igApp::Options& igOptionsDialog::Options( void )
{
	return options;
}

// igOptions.cpp