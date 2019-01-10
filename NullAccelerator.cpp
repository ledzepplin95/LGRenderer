#include "StdAfx.h"
#include "NullAccelerator.h"
#include "PrimitiveList.h"

LG_IMPLEMENT_DYNCREATE(NullAccelerator,AccelerationStructure)

NullAccelerator::NullAccelerator() 
{	
	n=0;
	primitives=NULL;
}

void NullAccelerator::build(PrimitiveList*p) 
{
	primitives=p;
	n=p->getNumPrimitives();
}

void NullAccelerator::intersect(Ray&r,IntersectionState&state) const
{
	for(int i=0; i<n; i++)
		primitives->intersectPrimitive(r,i,state);
}