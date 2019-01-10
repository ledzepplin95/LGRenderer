#pragma once
#include "filter.h"

class GaussianFilter :
	public Filter
{
	LG_DECLARE_DYNCREATE(GaussianFilter);

public:
    GaussianFilter();

	float getSize()const;
	float get(float x,float y)const;

private:
	float es2;
};
