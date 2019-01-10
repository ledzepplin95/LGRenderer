#pragma once
#include "filter.h"

class BoxFilter :
	public Filter
{
	LG_DECLARE_DYNCREATE(BoxFilter);

public:
	float getSize()const;
	float get(float x,float y)const;
};
