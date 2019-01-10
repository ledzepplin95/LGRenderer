#pragma once
#include "primitivelist.h"
#include "parameterlist.h"

class Hair :
	public PrimitiveList
{	
	LG_DECLARE_DYNCREATE(Hair);

public:
	Hair();

	int getNumPrimitives()const;
	float getPrimitiveBound(int primID,int i)const;
	BoundingBox getWorldBounds(const Matrix4&o2w)const;
	PrimitiveList* getBakingPrimitives()const;
	void intersectPrimitive(Ray&r,int primID,IntersectionState&state)const;

    void prepareShadingState(ShadingState&state);
	BOOL update(ParameterList&pl,LGAPI&api);
	Color getRadiance(ShadingState&state)const;
	void scatterPhoton(ShadingState&state,Color&power);	

private:
	int numSegments;
	vector<float> points;
	ParameterList::FloatParameter widths;

	float getWidth(int i)const;
	Vector3 getTangent(int line,int v0,float v)const;
};
