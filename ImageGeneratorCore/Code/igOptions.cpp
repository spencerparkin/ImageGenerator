// igOptions.cpp

#include "igHeader.h"

//===========================================================================
igOptionsDialog::igOptionsDialog( wxWindow* parent, const igApp::Options& options ) : wxDialog( parent, wxID_ANY, "Options" )
{
	this->options = options;

	wxIntegerValidator< int > imageWidthValidator( &this->options.imageSize.x );
	wxIntegerValidator< int > imageHeightValidator( &this->options.imageSize.y );
	wxIntegerValidator< int > threadCountValidator( &this->options.threadCount );

	imageWidthValidator.SetMin( 1 );
	imageHeightValidator.SetMin( 1 );

	threadCountValidator.SetMin( 0 );
	threadCountValidator.SetMax( 32 );

	wxTextCtrl* imageWidthText = new wxTextCtrl( this, wxID_ANY, wxEmptyString,
										wxPoint( 20, 20 ), wxDefaultSize, 0,
										imageWidthValidator );
	wxTextCtrl* imageHeightText = new wxTextCtrl( this, wxID_ANY, wxEmptyString,
										wxPoint( 150, 20 ), wxDefaultSize, 0,
										imageHeightValidator );
	wxTextCtrl* threadCountText = new wxTextCtrl( this, wxID_ANY, wxEmptyString,
										wxPoint( 20, 20 ), wxDefaultSize, 0,
										threadCountValidator );

	wxStaticBoxSizer* imageStaticBox = new wxStaticBoxSizer( wxHORIZONTAL, this, "Image" );
	imageStaticBox->Add( imageWidthText );
	imageStaticBox->Add( imageHeightText );

	wxStaticBoxSizer* threadStaticBox = new wxStaticBoxSizer( wxHORIZONTAL, this, "Thread" );
	threadStaticBox->Add( threadCountText );

	wxBoxSizer* boxSizer = new wxBoxSizer( wxVERTICAL );
	boxSizer->Add( imageStaticBox, 1, wxEXPAND | wxALL, 10 );
	boxSizer->Add( threadStaticBox, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10 );

	wxSizer* sizer = CreateSeparatedButtonSizer( wxOK | wxCANCEL );
	if( sizer )
		boxSizer->Add( sizer );
	
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