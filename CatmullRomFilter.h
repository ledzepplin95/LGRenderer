#pragma once
#include "filter.h"

class CatmullRomFilter :
	public Filter
{
	LG_DECLARE_DYNCREATE(CatmullRomFilter);

public:
	float getSize()const;
	float get(float x,float y)const;

private:
    float catrom1d(float x)const;
};
