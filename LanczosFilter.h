#pragma once
#include "filter.h"

class LanczosFilter :
	public Filter
{
    LG_DECLARE_DYNCREATE(LanczosFilter);

public:
	float getSize()const;
	float get(float x,float y)const;

private:
    float sinc1d(float x)const;	
};
