#pragma once

#include "lgobject.h"

class AccelerationStructure : public LGObject 
{
    LG_DECLARE_DYNCREATE(AccelerationStructure);
public:
	virtual void build(PrimitiveList*primitives);
	virtual void intersect(Ray&r,IntersectionState&istate)const;
};
