#include "StdAfx.h"
#include "CubicBSpline.h"

LG_IMPLEMENT_DYNCREATE(CubicBSpline,Filter)

float CubicBSpline::get(float x,float y) const
{
	return B3(x)*B3(y);
}

float CubicBSpline::getSize()const
{
	return 4.0f;
}

float CubicBSpline::B3(float t) const
{
	t=fabs(t);
	if(t<=1.0f)
		return b1(1-t);

	return b0(2.0f-t);
}

float CubicBSpline::b0(float t) const
{
	return t*t*t*(1.0f/6.0f);
}

float CubicBSpline::b1(float t) const
{
	return (1.0f/6.0f)*(-3.0f*t*t*t+3*t*t+3*t+1);
}
