#pragma once
#include "primitivelist.h"

class JuliaFractal :
	public PrimitiveList
{	
	LG_DECLARE_DYNCREATE(JuliaFractal);

public:
	JuliaFractal(void);
	
    int getNumPrimitives()const;
	float getPrimitiveBound(int primID,int i)const;
	BoundingBox getWorldBounds(const Matrix4&o2w)const;
	PrimitiveList* getBakingPrimitives()const;
	void intersectPrimitive(Ray&r,int primID,IntersectionState&state)const;

	void prepareShadingState(ShadingState&state);
	BOOL update(ParameterList&pl,LGAPI&api);	

private:
	static float BOUNDING_RADIUS;
	static float BOUNDING_RADIUS2;
	static float ESCAPE_THRESHOLD;
	static float DELTA;
	
	float cx;
	float cy;
	float cz;
	float cw;
	int maxIterations;
	float epsilon;

	static float length(float w,float x,float y,float z);
};
