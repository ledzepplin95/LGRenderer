#pragma once

#include "AccelerationStructure.h"

class NullAccelerator : public AccelerationStructure
{
	LG_DECLARE_DYNCREATE(NullAccelerator);

public:
	NullAccelerator(void);

	void build(PrimitiveList*p);
	void intersect(Ray&r,IntersectionState&state)const;
	
private:
	PrimitiveList* primitives;
	int n;
};
