// CylindricalInversion.h

//===========================================================================
// The zero set of this polynomial is the inversion of a cylinder in a sphere.
class CylindricalInversion : public Polynomial
{
public:

	CylindricalInversion( void );
	virtual ~CylindricalInversion( void );

	virtual Polynomial* Clone( void ) const override;
	virtual bool Configure( wxXmlNode* xmlNode ) override;

	virtual double Evaluate( const c3ga::vectorE3GA& point ) const override;

	// f(x,y,z) = x^4 + y^4 + z^4 + Ax( x^2 + y^2 + z^2 ) + B( x^2 + z^2 ) + 2x^2( y^2 + z^2 )

	// f_x(x,y,z) = 4x^3 + 3Ax^2 + Ay^2 + Az^2 + 2Bx + 4x( y^2 + z^2 )

	// f_y(x,y,z) = 4y^3 + 2Axy + 4x^2y

	// f_z(x,y,z) = 4z^3 + 2Axz + 2Bz + 4x^2z

	virtual double EvaluatePartialX( const c3ga::vectorE3GA& point ) const override;
	virtual double EvaluatePartialY( const c3ga::vectorE3GA& point ) const override;
	virtual double EvaluatePartialZ( const c3ga::vectorE3GA& point ) const override;

	double A;
	double B;
};

// CylindricalInversion.h