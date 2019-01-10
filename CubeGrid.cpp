#include "StdAfx.h"

#include "CubeGrid.h"
#include "ShadingState.h"
#include "float.h"
#include "ParameterList.h"
#include "instance.h"

LG_IMPLEMENT_DYNCREATE(CubeGrid,PrimitiveList)

CubeGrid::CubeGrid() 
{
	nx=ny=nz=1;
	bounds.set(1);
}

BOOL CubeGrid::update(ParameterList&pl,LGAPI&api)
{
	nx=pl.getInt("resolutionX",nx);
	ny=pl.getInt("resolutionY",ny);
	nz=pl.getInt("resolutionZ",nz);
	voxelwx=2.0f/nx;
	voxelwy=2.0f/ny;
	voxelwz=2.0f/nz;
	invVoxelwx=1.0f/voxelwx;
	invVoxelwy=1.0f/voxelwy;
	invVoxelwz=1.0f/voxelwz;

	return TRUE;
}

BoundingBox CubeGrid::getBounds()const 
{
	return bounds;
}

void CubeGrid::prepareShadingState(ShadingState&state) 
{
	state.init();
	state.getRay().getPoint(state.ss_point3());
	Instance* parent=state.getInstance();
	Vector3 normal;
	switch(state.getPrimitiveID())
	{
	case 0:
		normal.set(-1,0,0);
		break;
	case 1:
		normal.set(1,0,0);
		break;
	case 2:
		normal.set(0,-1,0);
		break;
	case 3:
		normal.set(0,1,0);
		break;
	case 4:
		normal.set(0,0,-1);
		break;
	case 5:
		normal.set(0,0,1);
		break;
	default:
		normal.set(0,0,0);
		break;
	}
	state.ss_normal().set(state.transformNormalObjectToWorld(normal));
	state.ss_geoNormal().set(state.getNormal());
	state.setBasis(OrthoNormalBasis::makeFromW(state.getNormal()));
	state.setShader(parent->getShader(0));
	state.setModifier(parent->getModifier(0));
}

void CubeGrid::intersectPrimitive(Ray&r,int primID,IntersectionState&state)const 
{
	float intervalMin=r.getMin();
	float intervalMax=r.getMax();
	float orgX=r.ox;
	float orgY=r.oy;
	float orgZ=r.oz;
	float dirX=r.dx,invDirX=1.0f/dirX;
	float dirY=r.dy,invDirY=1.0f/dirY;
	float dirZ=r.dz,invDirZ=1.0f/dirZ;
	float t1,t2;
	t1=(-1.0f-orgX)*invDirX;
	t2=(+1.0f-orgX)*invDirX;
	int curr=-1;
	if(invDirX>0.0f) 
	{
		if(t1>intervalMin) 
		{
			intervalMin=t1;
			curr=0;
		}
		if(t2<intervalMax)
			intervalMax=t2;
		if (intervalMin>intervalMax)
			return;
	} 
	else 
	{
		if(t2>intervalMin) 
		{
			intervalMin=t2;
			curr=1;
		}
		if(t1<intervalMax)
			intervalMax=t1;
		if(intervalMin>intervalMax)
			return;
	}
	t1=(-1.0-orgY)*invDirY;
	t2=(+1.0-orgY)*invDirY;
	if(invDirY>0.0f) 
	{
		if(t1>intervalMin)
		{
			intervalMin=t1;
			curr=2;
		}
		if(t2<intervalMax)
			intervalMax=t2;
		if (intervalMin>intervalMax)
			return;
	} 
	else 
	{
		if (t2>intervalMin)
		{
			intervalMin=t2;
			curr=3;
		}
		if(t1<intervalMax)
			intervalMax=t1;
		if (intervalMin>intervalMax)
			return;
	}
	t1=(-1.0-orgZ)*invDirZ;
	t2=(+1.0-orgZ)*invDirZ;
	if (invDirZ>0.0f) 
	{
		if(t1>intervalMin) 
		{
			intervalMin=t1;
			curr=4;
		}
		if (t2<intervalMax)
			intervalMax=t2;
		if (intervalMin>intervalMax)
			return;
	}
	else
	{
		if(t2>intervalMin) 
		{
			intervalMin=t2;
			curr=5;
		}
		if (t1<intervalMax)
			intervalMax=t1;
		if (intervalMin>intervalMax)
			return;
	}	
	orgX+=intervalMin*dirX;
	orgY+=intervalMin*dirY;
	orgZ+=intervalMin*dirZ;	
	int indxX,indxY,indxZ;
	int stepX,stepY,stepZ;
	int stopX,stopY,stopZ;
	float deltaX,deltaY,deltaZ;
	float tnextX,tnextY,tnextZ;	
	indxX=(int)((orgX+ 1)*invVoxelwx);
	if(indxX<0)
		indxX=0;
	else if(indxX>=nx)
		indxX=nx-1;
	if(fabs(dirX)<1e-6f)
	{
		stepX=0;
		stopX= indxX;
		deltaX=0;
		tnextX=FLT_MAX;
	} 
	else if(dirX>0.0f)
	{
		stepX=1;
		stopX=nx;
		deltaX=voxelwx*invDirX;
		tnextX=intervalMin+((indxX+1)*voxelwx-1-orgX)*invDirX;
	} 
	else
	{
		stepX=-1;
		stopX=-1;
		deltaX=-voxelwx*invDirX;
		tnextX=intervalMin+(indxX*voxelwx-1-orgX)*invDirX;
	}
	indxY=(int)((orgY+1)*invVoxelwy);
	if(indxY<0)
		indxY=0;
	else if(indxY>=ny)
		indxY=ny-1;
	if(fabs(dirY)<1e-6f)
	{
		stepY=0;
		stopY=indxY;
		deltaY=0;
		tnextY=FLT_MAX;
	} 
	else if(dirY>0.0f) 
	{
		stepY=1;
		stopY=ny;
		deltaY=voxelwy*invDirY;
		tnextY=intervalMin+((indxY+1)*voxelwy-1-orgY)*invDirY;
	} 
	else 
	{
		stepY=-1;
		stopY=-1;
		deltaY=-voxelwy*invDirY;
		tnextY=intervalMin+(indxY*voxelwy-1-orgY)*invDirY;
	}	
	indxZ=(int) ((orgZ+1)*invVoxelwz);
	if(indxZ<0)
		indxZ=0;
	else if(indxZ>=nz)
		indxZ=nz-1;
	if(fabs(dirZ)<1e-6f)
	{
		stepZ=0;
		stopZ=indxZ;
		deltaZ=0;
		tnextZ=FLT_MAX;
	}
	else if(dirZ>0.0f)
	{
		stepZ=1;
		stopZ=nz;
		deltaZ=voxelwz*invDirZ;
		tnextZ=intervalMin+((indxZ+1)*voxelwz-1-orgZ)*invDirZ;
	} 
	else 
	{
		stepZ=-1;
		stopZ=-1;
		deltaZ=-voxelwz*invDirZ;
		tnextZ=intervalMin+(indxZ*voxelwz-1-orgZ)*invDirZ;
	}
	BOOL isInside=inside(indxX,indxY,indxZ)
		&& bounds.contains(r.ox,r.oy,r.oz);	
	for(;;) 
	{
		if( inside(indxX,indxY,indxZ)!=isInside )
		{		
			r.setMax(intervalMin);		
			if (isInside)
				curr^=1;
			state.setIntersection(curr);
			return;
		}
		if( tnextX<tnextY && tnextX<tnextZ )
		{
			curr=dirX>0.0f?0:1;
			intervalMin=tnextX;
			if(intervalMin>intervalMax)
				return;
			indxX+=stepX;
			if(indxX==stopX)
				return;
			tnextX+=deltaX;
		} 
		else if(tnextY<tnextZ) 
		{
			curr=dirY>0.0f?2:3;
			intervalMin=tnextY;
			if(intervalMin>intervalMax)
				return;
			indxY+=stepY;
			if(indxY==stopY)
				return;
			tnextY+=deltaY;
		} 
		else 
		{
			curr=dirZ>0.0f?4:5;
			intervalMin=tnextZ;
			if(intervalMin>intervalMax)
				return;
			indxZ+=stepZ;
			if(indxZ==stopZ)
				return;
			tnextZ+=deltaZ;
		}
	}
}

int CubeGrid::getNumPrimitives() const
{
	return 1;
}

float CubeGrid::getPrimitiveBound(int primID,int i)const
{
	return ((i&1)==0)?-1.0f:1.0f;
}

BoundingBox CubeGrid::getWorldBounds(const Matrix4&o2w) const
{
	return o2w.transform(bounds);
}

BOOL CubeGrid::inside(int x,int y,int z) const
{
	return FALSE;
}