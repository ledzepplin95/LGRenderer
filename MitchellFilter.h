#pragma once
#include "filter.h"

class MitchellFilter :
	public Filter
{
	LG_DECLARE_DYNCREATE(MitchellFilter);

public:
	float getSize()const;
	float get(float x,float y)const;

private:
	float mitchell(float x)const;	
};
