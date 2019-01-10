#pragma once

#include "renderobject.h" 

class PrimitiveList : public RenderObject
{
    LG_DECLARE_DYNCREATE(PrimitiveList);

public:
	PrimitiveList(){};
	virtual ~PrimitiveList(){};

	virtual void prepareShadingState(ShadingState&state);
	virtual void intersectPrimitive(Ray&r,int primID,IntersectionState&state)const;	
	virtual BoundingBox getWorldBounds(const Matrix4&o2w)const;
    virtual int getNumPrimitives()const;	
	virtual float getPrimitiveBound(int primID,int i)const;  	
	virtual PrimitiveList* getBakingPrimitives()const;
};



