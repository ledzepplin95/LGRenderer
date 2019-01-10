#include "stdafx.h"

#include "Tesselatable.h"
#include "BoundingBox.h"

LG_IMPLEMENT_DYNCREATE(Tesselatable,RenderObject)

PrimitiveList* Tesselatable::tesselate()const
{
	return NULL;
}

BoundingBox Tesselatable::getWorldBounds(const Matrix4&o2w)const
{
	return BoundingBox(0.0f);
}