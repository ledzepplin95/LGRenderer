#pragma once
#include "renderobject.h"

class Modifier :
	public RenderObject
{
    LG_DECLARE_DYNCREATE(Modifier);
public:
	virtual void modify(ShadingState&state)const;
	BOOL operator==(const Modifier&vec) const; 
};
