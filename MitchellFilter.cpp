#include "StdAfx.h"
#include "MitchellFilter.h"

LG_IMPLEMENT_DYNCREATE(MitchellFilter,Filter)

float MitchellFilter::getSize() const 
{
	return 4.0f;
}

float MitchellFilter::get(float x,float y)const
{
	return mitchell(x)*mitchell(y);
}

float MitchellFilter::mitchell(float x) const
{
	float B=1.0f/3.0f;
	float C=1.0f/3.0f;
	float SIXTH=1.0f/6.0f;
	x=fabs(x);
	float x2=x*x;
	if(x>1.0f)
		return ((-B-6.0*C)*x*x2+(6.0*B+30.0*C)*x2 
		+(-12.0*B-48.0*C)*x+(8.0*B+24.0*C))*SIXTH;

	return ((12.0-9.0*B-6.0*C)*x*x2+
		(-18.0+12.0*B+6.0*C)*x2+(6.0-2.0*B))*SIXTH;
}
