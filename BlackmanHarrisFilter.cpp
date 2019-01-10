#include "StdAfx.h"

#include "BlackmanHarrisFilter.h"
#include "LGdef.h"

LG_IMPLEMENT_DYNCREATE(BlackmanHarrisFilter,Filter)

float BlackmanHarrisFilter::getSize()const
{
	return 4.0f;
}

float BlackmanHarrisFilter::get(float x,float y) const
{
	return bh1d(x*0.5f)*bh1d(y*0.5f);
}

float BlackmanHarrisFilter::bh1d(float x)const 
{
	if( x<-1.0f || x>1.0f )
		return 0.0f;
	x=(x+1.0)*0.5f;
	double A0=0.35875;
	double A1=-0.48829;
	double A2=0.14128;
	double A3=-0.01168;

	return (float)(A0+A1*cos(2.0*LG_PI*x) 
		+A2*cos(4.0*LG_PI*x)+A3*cos(6.0*LG_PI*x));
}