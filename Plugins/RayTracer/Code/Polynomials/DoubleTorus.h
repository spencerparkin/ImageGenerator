// DoubleTorus.h

//===========================================================================
class DoubleTorus : public Polynomial
{
public:

	DoubleTorus( void );
	virtual ~DoubleTorus( void );

	virtual Polynomial* Clone( void ) const override;
	virtual bool Configure( wxXmlNode* xmlNode ) override;

	virtual double Evaluate( const c3ga::vectorE3GA& point ) const override;

	virtual double EvaluatePartialX( const c3ga::vectorE3GA& point ) const override;
	virtual double EvaluatePartialY( const c3ga::vectorE3GA& point ) const override;
	virtual double EvaluatePartialZ( const c3ga::vectorE3GA& point ) const override;
};

// DoubleTorus.h