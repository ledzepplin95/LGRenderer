#pragma once
#include "filter.h"

class CubicBSpline :
	public Filter
{
	LG_DECLARE_DYNCREATE(CubicBSpline);

public:
	float get(float x,float y)const;
	float getSize()const;

private:
	float B3(float t)const;
	float b0(float t)const;
	float b1(float t)const;	
};
