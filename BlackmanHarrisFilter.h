#pragma once
#include "filter.h"

class BlackmanHarrisFilter :
	public Filter
{
	LG_DECLARE_DYNCREATE(BlackmanHarrisFilter);

public:
	float get(float x,float y)const;
	float getSize()const;

private:
	float bh1d(float x)const;	
};
