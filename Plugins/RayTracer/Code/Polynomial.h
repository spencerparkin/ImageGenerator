// Polynomial.h

#pragma once

#include <stdio.h>
#include "c3ga/c3ga.h"
#include <wx/xml/xml.h>

//===========================================================================
// Here we mean a polynomial in three variables x, y and z.
// Class derivatives determine the degree.
class Polynomial
{
public:

	Polynomial( void );
	virtual ~Polynomial( void );

	virtual Polynomial* Clone( void ) const = 0;
	virtual bool Configure( wxXmlNode* xmlNode );

	virtual double Evaluate( const c3ga::vectorE3GA& point ) const = 0;
	virtual double EvaluatePartialX( const c3ga::vectorE3GA& point ) const;
	virtual double EvaluatePartialY( const c3ga::vectorE3GA& point ) const;
	virtual double EvaluatePartialZ( const c3ga::vectorE3GA& point ) const;
	virtual void EvaluateGradient( const c3ga::vectorE3GA& point, c3ga::vectorE3GA& gradient ) const;
};

// Polynomial.h