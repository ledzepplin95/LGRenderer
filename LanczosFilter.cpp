#include "StdAfx.h"

#include "LanczosFilter.h"
#include "LGdef.h"

LG_IMPLEMENT_DYNCREATE(LanczosFilter,Filter)

float LanczosFilter::getSize() const
{
	return 4.0f;
}

float LanczosFilter::get(float x, float y) const
{
	return sinc1d(x*0.5f)*sinc1d(y*0.5f);
}

float LanczosFilter::sinc1d(float x) const
{
	x=fabs(x);
	if(x<1e-5f)
		return 1.0f;
	if(x>1.0f)
		return 0.0f;
	x*=LG_PI;
	float sinc=(float)sin(3.0*x)/(3.0*x);
	float lanczos=(float)sin(x)/x;

	return sinc*lanczos;
}
