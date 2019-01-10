#pragma once
#include "tesselatable.h"

class BezierMesh :
	public Tesselatable
{
	LG_DECLARE_DYNCREATE(BezierMesh);

public:
	BezierMesh();
	BezierMesh(const vector<vector<float>>&p);
    BezierMesh(float p[][49]);
	BezierMesh(float p[][48]);

	BoundingBox getWorldBounds(const Matrix4&o2w)const;
    PrimitiveList* tesselate()const;
	BOOL update(ParameterList&pl,LGAPI&api);

private:
    int subdivs;
	BOOL smooth;
	BOOL quads;
	vector<vector<float>> patches;

	vector<float> bernstein(float u)const;
	vector<float> bernsteinDeriv(float u)const;
	void getPatchPoint(float u,float v,const vector<float>&ctrl,const vector<float>&bu, 
		const vector<float>&bv,const vector<float>&bdu,const vector<float>&bdv,
		Point3&p,Vector3&n)const;	
};
