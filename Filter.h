#pragma once

#include "lgobject.h"

class Filter : public LGObject 
{
	LG_DECLARE_DYNCREATE(Filter);
public:
    virtual float getSize()const;
	virtual float get(float x,float y)const;	
};
