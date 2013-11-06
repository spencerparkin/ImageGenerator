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

	// f(x,y,z) = 16x^4 - 32x^6 - 8x^2y^2 + 16x^8 + 8x^4y^2 + y^4 + z^2 + 0.25

	virtual double EvaluatePartialX( const c3ga::vectorE3GA& point ) const override;
	virtual double EvaluatePartialY( const c3ga::vectorE3GA& point ) const override;
	virtual double EvaluatePartialZ( const c3ga::vectorE3GA& point ) const override;
};

// DoubleTorus.h