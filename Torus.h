#pragma once
#include "primitivelist.h"

class Torus :
	public PrimitiveList
{	
	LG_DECLARE_DYNCREATE(Torus);

public:
	Torus();

	BOOL update(ParameterList&pl,LGAPI&api);
	void prepareShadingState(ShadingState&state);

	void intersectPrimitive(Ray&r,int primID,IntersectionState&state)const;
	BoundingBox getWorldBounds(const Matrix4&o2w)const;
    float getPrimitiveBound(int primID,int i)const;
	int getNumPrimitives()const;
	PrimitiveList* getBakingPrimitives()const;

private:
	float ri2,ro2;
	float ri,ro;	
};
