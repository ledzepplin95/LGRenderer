#pragma once

#include "primitivelist.h"
#include "Vector3.h"

class SphereFlake :
	public PrimitiveList
{	
	LG_DECLARE_DYNCREATE(SphereFlake);

public:
	SphereFlake();

	BOOL update(ParameterList&pl,LGAPI&api);
	void prepareShadingState(ShadingState&state);

	void intersectPrimitive(Ray&r,int primID,IntersectionState&state)const;
	BoundingBox getWorldBounds(const Matrix4&o2w)const;
	float getPrimitiveBound(int primID,int i)const;
	int getNumPrimitives()const;	
	PrimitiveList* getBakingPrimitives()const;
	static void initial();

private:
	static int MAX_LEVEL;
	static vector<float> boundingRadiusOffset;
	static vector<float> recursivePattern;	

	int level;
	Vector3 axis;
	float baseRadius;

	void intersectFlake(Ray&r,IntersectionState&state,int level, 
		float qa,float qaInv,float cx,float cy,
		float cz,float dx,float dy,float dz,float radius)const;	
};
