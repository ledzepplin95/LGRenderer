#pragma once

#include "renderobject.h"

class Tesselatable : public RenderObject
{
	LG_DECLARE_DYNCREATE(Tesselatable);
public:
	virtual PrimitiveList* tesselate()const;
	virtual BoundingBox getWorldBounds(const Matrix4&o2w)const;	
};
