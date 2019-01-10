#include "stdafx.h"

#include "PrimitiveList.h"
#include "BoundingBox.h"
#include "ray.h"

LG_IMPLEMENT_DYNCREATE(PrimitiveList,RenderObject)

void PrimitiveList::intersectPrimitive(Ray&r,int primID,IntersectionState&state)const
{
}

void PrimitiveList::prepareShadingState(ShadingState&state)
{
}

BoundingBox PrimitiveList::getWorldBounds(const Matrix4&o2w)const
{
	return BoundingBox(0.0f);
}

int PrimitiveList::getNumPrimitives()const
{
	return 0;
}

float PrimitiveList::getPrimitiveBound(int primID,int i)const
{
	return 0.0f;
}

PrimitiveList* PrimitiveList::getBakingPrimitives()const
{
	return NULL;
}