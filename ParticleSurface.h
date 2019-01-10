#pragma once
#include "primitivelist.h"

class ParticleSurface :
	public PrimitiveList
{	
	LG_DECLARE_DYNCREATE(ParticleSurface);

public:
	ParticleSurface(void);

	int getNumPrimitives()const;
	float getPrimitiveBound(int primID,int i)const;
	BoundingBox getWorldBounds(const Matrix4&o2w)const;
	PrimitiveList* getBakingPrimitives()const;
	void intersectPrimitive(Ray&r,int primID,IntersectionState&state)const;

	void prepareShadingState(ShadingState&state);
	BOOL update(ParameterList&pl,LGAPI&api);	

private:
	vector<float> particles;
	float r,r2;
	int n;	
};
