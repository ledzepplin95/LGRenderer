#include "StdAfx.h"
#include "CatmullRomFilter.h"

LG_IMPLEMENT_DYNCREATE(CatmullRomFilter,Filter)

float CatmullRomFilter::getSize() const
{
	return 4.0f;
}

float CatmullRomFilter::get(float x,float y) const
{
	return catrom1d(x)*catrom1d(y);
}

float CatmullRomFilter::catrom1d(float x)const 
{
	x=fabs(x);
	float x2=x*x;
	float x3=x*x2;
	if(x>=2.0f)
		return 0.0f;
	if(x<1.0f)
		return 3.0f*x3-5.0f*x2+2.0f;

	return -x3+5.0f*x2-8.0f*x+4.0f;
}
