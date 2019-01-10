#pragma once
#include "primitivelist.h"
#include "parameterlist.h"

class TriangleMesh;

class WaldTriangle
{
public:
	int k;
	float nu,nv,nd;
	float bnu,bnv,bnd;
	float cnu,cnv,cnd;

	WaldTriangle();
	WaldTriangle(const TriangleMesh&mesh,int tri);

	void set(const TriangleMesh&mesh,int t);
	WaldTriangle& operator=(const WaldTriangle&wt);

	void intersect(Ray&r,int primID,IntersectionState&state)const;		
};

class BakingSurface : public PrimitiveList 
{
	LG_DECLARE_DYNCREATE(BakingSurface);
public:
	BakingSurface();
	BakingSurface(TriangleMesh*m);

	PrimitiveList* getBakingPrimitives()const;
	int getNumPrimitives()const;
	float getPrimitiveBound(int primID,int i)const;
	BoundingBox getWorldBounds(const Matrix4&o2w)const;
	void intersectPrimitive(Ray&r,int primID,IntersectionState&state)const;

	void prepareShadingState(ShadingState&state);
	BOOL update(ParameterList&pl,LGAPI&api);

private:		
	TriangleMesh*t_mesh;		
};

class TriangleMesh :
	public PrimitiveList
{
	LG_DECLARE_DYNCREATE(TriangleMesh);

public:
	static void setSmallTriangles(BOOL sTriangles);

	TriangleMesh();
	void writeObj(CString fileName);
	BOOL update(ParameterList&pl,LGAPI&api);    
	void prepareShadingState(ShadingState&state);
	void init();

	void intersectPrimitive(Ray&r,int primID,IntersectionState&state)const;
	Point3 getPoint(int i)const;
	Point3 getPoint(int tri,int i)const;
    int getNumPrimitives()const;
	PrimitiveList* getBakingPrimitives()const;
	float getPrimitiveBound(int primID,int i)const;
	BoundingBox getWorldBounds(const Matrix4&o2w)const;

public:
	vector<float> points;
	vector<int> triangles;

public:
	static BOOL smallTriangles;

	vector<WaldTriangle> triAccel;
	ParameterList::FloatParameter normals;
	ParameterList::FloatParameter uvs;
	vector<byte> faceShaders;

	void intersectTriangleKensler(Ray&r,int primID,IntersectionState&state)const;
};
