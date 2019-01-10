#include "StdAfx.h"

#include "Box.h"
#include "float.h"
#include "ShadingState.h"
#include "parameterlist.h"
#include "BoundingBox.h"
#include "Instance.h"

LG_IMPLEMENT_DYNCREATE(Box,PrimitiveList)

Box::Box() 
{
	minX=minY=minZ=-1;
	maxX=maxY=maxZ=+1;
}

BOOL Box::update(ParameterList&pl,LGAPI&api) 
{
	ParameterList::FloatParameter pts=pl.getPointArray("points");
	BoundingBox bounds(0.0f);
	for(int i=0; i<pts.data.size(); i+=3)
		bounds.include(pts.data[i],pts.data[i+1],pts.data[i+2]);		
	minX=bounds.getMinimum().x;
	minY=bounds.getMinimum().y;
	minZ=bounds.getMinimum().z;
	maxX=bounds.getMaximum().x;
	maxY=bounds.getMaximum().y;
	maxZ=bounds.getMaximum().z;
	
	return TRUE;
}

void Box::prepareShadingState(ShadingState&state) 
{
	state.init();
	state.getRay().getPoint(state.ss_point3());
	int n=state.getPrimitiveID();
	switch(n) 
	{
	case 0:
		state.ss_normal().set(Vector3(1,0,0));
		break;
	case 1:
		state.ss_normal().set(Vector3(-1,0,0));
		break;
	case 2:
		state.ss_normal().set(Vector3(0,1,0));
		break;
	case 3:
		state.ss_normal().set(Vector3(0,-1,0));
		break;
	case 4:
		state.ss_normal().set(Vector3(0,0,1));
		break;
	case 5:
		state.ss_normal().set(Vector3(0,0,-1));
		break;
	default:
		state.ss_normal().set(Vector3(0,0,0));
		break;
	}
	state.ss_geoNormal().set(state.getNormal());
	state.setBasis(OrthoNormalBasis::makeFromW(state.getNormal()));
	state.setShader(state.getInstance()->getShader(0));
	state.setModifier(state.getInstance()->getModifier(0));
}

void Box::intersectPrimitive(Ray&r,int primID,IntersectionState&state)const 
{
	float intervalMin=-FLT_MAX;
	float intervalMax=FLT_MAX;
	float orgX=r.ox;
	float invDirX=1.0f/r.dx;
	float t1,t2;
	t1=(minX-orgX)*invDirX;
	t2=(maxX-orgX)*invDirX;
	int sideIn=-1,sideOut=-1;
	if(invDirX>0.0f)
	{
		if(t1>intervalMin)
		{
			intervalMin=t1;
			sideIn=0;
		}
		if(t2<intervalMax)
		{
			intervalMax=t2;
			sideOut=1;
		}
	} 
	else 
	{
		if(t2>intervalMin) 
		{
			intervalMin=t2;
			sideIn=1;
		}
		if(t1<intervalMax) 
		{
			intervalMax=t1;
			sideOut=0;
		}
	}
	if(intervalMin>intervalMax)
		return;
	float orgY=r.oy;
	float invDirY=1.0f/r.dy;
	t1=(minY-orgY)*invDirY;
	t2=(maxY-orgY)*invDirY;
	if(invDirY>0.0f) 
	{
		if(t1>intervalMin) 
		{
			intervalMin=t1;
			sideIn=2;
		}
		if(t2<intervalMax) 
		{
			intervalMax=t2;
			sideOut=3;
		}
	} 
	else 
	{
		if(t2>intervalMin)
		{
			intervalMin=t2;
			sideIn=3;
		}
		if(t1<intervalMax)
		{
			intervalMax=t1;
			sideOut=2;
		}
	}
	if(intervalMin>intervalMax)
		return;
	float orgZ=r.oz;
	float invDirZ=1.0f/r.dz;
	t1=(minZ-orgZ)*invDirZ;
	t2=(maxZ-orgZ)*invDirZ;
	if(invDirZ>0.0f)
	{
		if(t1>intervalMin) 
		{
			intervalMin=t1;
			sideIn=4;
		}
		if(t2<intervalMax) 
		{
			intervalMax=t2;
			sideOut=5;
		}
	} 
	else 
	{
		if(t2>intervalMin)
		{
			intervalMin=t2;
			sideIn=5;
		}
		if(t1<intervalMax) 
		{
			intervalMax=t1;
			sideOut=4;
		}
	}
	if(intervalMin>intervalMax)
		return;
	if(r.isInside(intervalMin))
	{
		r.setMax(intervalMin);
		state.setIntersection(sideIn);
	} 
	else if(r.isInside(intervalMax)) 
	{
		r.setMax(intervalMax);
		state.setIntersection(sideOut);
	}
}

int Box::getNumPrimitives() const
{
	return 1;
}

float Box::getPrimitiveBound(int primID,int i)const 
{
	switch(i) 
	{
	case 0:
		return minX;
	case 1:
		return maxX;
	case 2:
		return minY;
	case 3:
		return maxY;
	case 4:
		return minZ;
	case 5:
		return maxZ;
	default:
		return 0.0f;
	}
}

BoundingBox Box::getWorldBounds(const Matrix4&o2w) const
{
	BoundingBox bounds(minX,minY,minZ);
	bounds.include(maxX,maxY,maxZ);	

	return o2w.transform(bounds);
}

PrimitiveList* Box::getBakingPrimitives() const
{
	return NULL;
}