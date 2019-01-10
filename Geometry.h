#pragma once

#include "renderobject.h"

class Geometry :
	public RenderObject
{
	LG_DECLARE_DYNCREATE(Geometry);

public:
	Geometry(void);
	Geometry(Tesselatable*tess);
	Geometry(PrimitiveList*prim);
    
	void set(Tesselatable*tess);
	void set(PrimitiveList*prim);
    BOOL update(ParameterList&pl,LGAPI&api);	
	void prepareShadingState(ShadingState&state);	
	void intersect(Ray&r,IntersectionState&state);
	BoundingBox getWorldBounds(const Matrix4&o2w);	
	PrimitiveList* getBakingPrimitives();

	int getNumPrimitives()const;
	PrimitiveList* getPrimitiveList()const;
	BOOL operator ==(const Geometry&geo) const;

private:
	Tesselatable* tesselatable;
	PrimitiveList* primitives;
	AccelerationStructure* accel;
	int builtAccel;
	int builtTess;
	CString acceltype;
	CCriticalSection cs;

	void tesselate();
    void build();
};
