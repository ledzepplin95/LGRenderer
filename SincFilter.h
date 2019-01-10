#pragma once
#include "filter.h"

class SincFilter :
	public Filter
{
	LG_DECLARE_DYNCREATE(SincFilter);

public:
    float getSize()const;
	float get(float x,float y)const;

private:
	float sinc1d(float x)const;	
};
