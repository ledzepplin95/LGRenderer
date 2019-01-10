#pragma once
#include "filter.h"

class TriangleFilter :
	public Filter
{
    LG_DECLARE_DYNCREATE(TriangleFilter);

public:
	float getSize()const;
	float get(float x,float y)const;	
};
