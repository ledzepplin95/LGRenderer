#pragma once
#include "primitivelist.h"
#include "point3.h"
#include "vector3.h"

class Plane :
	public PrimitiveList
{
	LG_DECLARE_DYNCREATE(Plane);

public:
	Plane();

	BOOL update(ParameterList&pl, LGAPI&api);
	void prepareShadingState(ShadingState&state);

	void intersectPrimitive(Ray&r,int primID,IntersectionState&state)const;
	int getNumPrimitives()const;
	float getPrimitiveBound(int primID,int i)const;
	BoundingBox getWorldBounds(const Matrix4&o2w)const;
	PrimitiveList* getBakingPrimitives()const;

public:
	int k;

private:
	Point3 center;
	Vector3 normal;
	
	float bnu,bnv,bnd;
	float cnu,cnv,cnd;
};
