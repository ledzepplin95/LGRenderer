#pragma once

#include "tesselatable.h"

class TriangleMesh;
class FileMesh :
	public Tesselatable
{
	LG_DECLARE_DYNCREATE(FileMesh);

public:
	FileMesh();

	BoundingBox getWorldBounds(const Matrix4&o2w)const;
	PrimitiveList* tesselate()const;
    BOOL update(ParameterList&pl,LGAPI&api);

private:
	CString fileName;
	BOOL smoothNormals;

	TriangleMesh* generate(const vector<int>&tris,const vector<float>&verts,BOOL smoothNormals)const;
	static int getLittleEndianInt(int i);
	static float getLittleEndianFloat(int i);
};
