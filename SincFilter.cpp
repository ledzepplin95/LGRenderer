#include "StdAfx.h"

#include "SincFilter.h"
#include "LGdef.h"

LG_IMPLEMENT_DYNCREATE(SincFilter,Filter)

float SincFilter::getSize() const
{
	return 4.0f;
}

float SincFilter::get(float x, float y)const
{
	return sinc1d(x)*sinc1d(y);
}

float SincFilter::sinc1d(float x)const 
{
	x=fabs(x);
	if(x<0.0001f)
		return 1.0f;
	x*=LG_PI;

	return (float)sin(x)/x;
}
