// RootFinder.h

//===========================================================================
class RootFinder
{
public:

	//===========================================================================
	// Being a continuous function, the intermediate value theorem applies.
	class ContinuousFunction
	{
	public:
		virtual double Evaluate( double lambda ) = 0;
	};

	RootFinder( ContinuousFunction* continuousFunction );
	~RootFinder( void );

	bool FindFirstRoot( double min, double max, double delta, double& root );

private:

	ContinuousFunction* continuousFunction;
};

// RootFinder.h