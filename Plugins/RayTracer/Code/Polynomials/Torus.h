// Torus.h

//===========================================================================
class Torus : public Polynomial
{
public:

	Torus( void );
	virtual ~Torus( void );

	virtual Polynomial* Clone( void ) const override;
	virtual bool Configure( wxXmlNode* xmlNode ) override;

	virtual double Evaluate( const c3ga::vectorE3GA& point ) const override;

	virtual double EvaluatePartialX( const c3ga::vectorE3GA& point ) const override;
	virtual double EvaluatePartialY( const c3ga::vectorE3GA& point ) const override;
	virtual double EvaluatePartialZ( const c3ga::vectorE3GA& point ) const override;
};

// Torus.h