#pragma once
#include "primitivelist.h"
#include "BoundingBox.h"

class CubeGrid :
	public PrimitiveList
{
	LG_DECLARE_DYNCREATE(CubeGrid);

public:
	CubeGrid();

	BOOL update(ParameterList&pl,LGAPI&api);	
	void prepareShadingState(ShadingState&state);

	BoundingBox getBounds()const;
	void intersectPrimitive(Ray&r,int primID,IntersectionState&state)const;
	int getNumPrimitives()const;
	float getPrimitiveBound(int primID,int i)const;
	BoundingBox getWorldBounds(const Matrix4&o2w)const;

protected:
	virtual BOOL inside(int x,int y,int z)const;	

private:
	int nx,ny,nz;
	float voxelwx,voxelwy,voxelwz;
	float invVoxelwx,invVoxelwy,invVoxelwz;
	BoundingBox bounds;	
};
