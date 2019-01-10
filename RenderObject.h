#pragma once

#include "LGObject.h"

class RenderObject : public LGObject 
{
    LG_DECLARE_DYNCREATE(RenderObject);
	
public:
	virtual ~RenderObject(){};
    virtual BOOL update(ParameterList&pl,LGAPI&api);  
};