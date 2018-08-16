// RootFinder.cpp

#include "RootFinder.h"
#include <math.h>

//===========================================================================
RootFinder::RootFinder( ContinuousFunction* continuousFunction )
{
	this->continuousFunction = continuousFunction;
}

//===========================================================================
RootFinder::~RootFinder( void )
{
}

//===========================================================================
// Whether we really find the first root here depends upon our sampling
// frequency being high enough.  I'm sure there's got to be a better way.
bool RootFinder::FindFirstRoot( double min, double max, double delta, double& root )
{
	double eval0, eval1;
	double lambda0, lambda1;

	lambda0 = lambda1 = min;
	eval0 = continuousFunction->Evaluate( lambda0 );
	if( eval0 == 0.0 )
		root = eval0;
	else
	{
		for(;;)
		{
			lambda1 = lambda0 + delta;
			eval1 = continuousFunction->Evaluate( lambda1 );
			if( eval1 == 0.0 )
			{
				root = eval1;
				break;
			}

			int sign0 = ( eval0 < 0.0 ) ? -1 : 1;
			int sign1 = ( eval1 < 0.0 ) ? -1 : 1;
			if( sign0 != sign1 )
			{
				// At this point we know that a root exists within
				// the open interval (lambda0, lambda1).  We are now
				// going to find one.  We may not necessarily find
				// the first one, but we will find one.
				double eps = 1e-9;
				for(;;)
				{
					double center = 0.5 * ( lambda0 + lambda1 );
					double centerEval = continuousFunction->Evaluate( center );
					if( fabs( centerEval ) < eps )
					{
						root = center;
						break;
					}
					
					int centerSign = ( centerEval < 0.0 ) ? -1 : 1;
					if( centerSign != sign0 )
					{
						eval1 = centerEval;
						lambda1 = center;
					}
					else
					{
						eval0 = centerEval;
						lambda0 = center;
					}
				}

				break;
			}

			lambda0 = lambda1;
			if( lambda0 > max )
				return false;
		}
	}

	return true;
}

// RootFinder.cpp