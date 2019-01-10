#pragma once
#include "primitivelist.h"

class Sphere :
	public PrimitiveList
{	
	LG_DECLARE_DYNCREATE(Sphere);

public:
	BOOL update(ParameterList&pl,LGAPI&api);
	void prepareShadingState(ShadingState&state);

	void intersectPrimitive(Ray&r,int primID,IntersectionState&state)const;
	BoundingBox getWorldBounds(const Matrix4&o2w)const;
	float getPrimitiveBound(int primID,int i)const;
	int getNumPrimitives()const;	
	PrimitiveList* getBakingPrimitives()const;	
};
