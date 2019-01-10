#pragma once

#include "AccelerationStructure.h"
#include "BoundingBox.h"
#include "IntArray.h"

class UniformGrid : public AccelerationStructure
{
	LG_DECLARE_DYNCREATE(UniformGrid);

public:
	UniformGrid(void);

    void build(PrimitiveList*p);
	void intersect(Ray&r,IntersectionState&state)const;

private:
	int nx,ny,nz;
	PrimitiveList* primitives;
	BoundingBox bounds;
	vector<IntArray> cells;
	float voxelwx,voxelwy,voxelwz;
	float invVoxelwx,invVoxelwy,invVoxelwz;

	void getGridIndex(float x,float y,float z,vector<int>&i)const;
};
