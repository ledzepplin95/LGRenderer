#pragma once
#include "primitivelist.h"

class Box :
	public PrimitiveList
{	
	LG_DECLARE_DYNCREATE(Box);

public:
	Box();

    BOOL update(ParameterList&pl,LGAPI&api);
	void prepareShadingState(ShadingState&state);

	void intersectPrimitive(Ray&r,int primID,IntersectionState&state)const;
	int getNumPrimitives()const;
	float getPrimitiveBound(int primID,int i)const;
	BoundingBox getWorldBounds(const Matrix4&o2w)const;
	PrimitiveList* getBakingPrimitives()const;

private:
	float minX,minY,minZ;
	float maxX,maxY,maxZ;	
};
