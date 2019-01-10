#pragma once
#include "primitivelist.h"
#include "parameterlist.h"

class QuadMesh :
	public PrimitiveList
{
    LG_DECLARE_DYNCREATE(QuadMesh);

public:
	QuadMesh();

	void writeObj(CString fileName);
    BOOL update(ParameterList&pl,LGAPI&api);
	void prepareShadingState(ShadingState&state);

	void intersectPrimitive(Ray&r,int primID,IntersectionState&state)const;
	Point3 getPoint(int i)const;
	float getPrimitiveBound(int primID,int i)const;
	BoundingBox getWorldBounds(const Matrix4&o2w)const;	
	int getNumPrimitives()const;	
	PrimitiveList* getBakingPrimitives()const;

private:
	vector<float> points;
	vector<int> quads;
	ParameterList::FloatParameter normals;
	ParameterList::FloatParameter uvs;
	vector<byte> faceShaders;	
};
