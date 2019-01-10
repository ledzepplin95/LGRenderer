#include "StdAfx.h"

#include "Plane.h"
#include "ShadingState.h"
#include "parameterlist.h"
#include "instance.h"
#include "strutil.h"

LG_IMPLEMENT_DYNCREATE(Plane,PrimitiveList)

Plane::Plane() 
{
	center.set(0,0,0);
	normal.set(0,1,0);
	k=3;
	bnu=bnv=bnd=0.0f;
	cnu=cnv=cnd=0.0f;
}

BOOL Plane::update(ParameterList&pl,LGAPI&api) 
{
	center=pl.getPoint("center",center);
	Point3 b=pl.getPoint("point1",Point3());
	Point3 c=pl.getPoint("point2",Point3());

    if(b!=Point3() && c!=Point3())
	{
		Point3 v0=center;
		Point3 v1=b;
		Point3 v2=c;
		Vector3 ng=normal=Vector3::cross(Point3::sub(v1,v0,Vector3()), 
			Point3::sub(v2,v0,Vector3()),Vector3()).normalize();
		if( fabs(ng.x)>fabs(ng.y) && fabs(ng.x)>fabs(ng.z) )
			k=0;
		else if( fabs(ng.y)>fabs(ng.z) )
			k=1;
		else
			k=2;
		float ax,ay,bx,by,cx,cy;
		switch(k) 
		{
		case 0: 
			{
				ax=v0.y;
				ay=v0.z;
				bx=v2.y-ax;
				by=v2.z-ay;
				cx=v1.y-ax;
				cy=v1.z-ay;
				break;
			}
		case 1:
			{
				ax=v0.z;
				ay=v0.x;
				bx=v2.z-ax;
				by=v2.x-ay;
				cx=v1.z-ax;
				cy=v1.x-ay;
				break;
			}
		case 2:
		default: 
			{
				ax=v0.x;
				ay=v0.y;
				bx=v2.x-ax;
				by=v2.y-ay;
				cx=v1.x-ax;
				cy=v1.y-ay;
			}
		}
		float det=bx*cy-by*cx;
		bnu=-by/det;
		bnv=bx/det;
		bnd=(by*ax-bx*ay)/det;
		cnu=cy/det;
		cnv=-cx/det;
		cnd=(cx*ay-cy*ax)/det;
	}
	else
	{
		normal=pl.getVector("normal",normal);
		k=3;
		bnu=bnv=bnd=0.0f;
		cnu=cnv=cnd=0.0f;	
	}
	
	return TRUE;
}

void Plane::prepareShadingState(ShadingState&state)
{
	state.init();
	state.getRay().getPoint(state.ss_point3());
	Instance* parent=state.getInstance();
	Vector3 worldNormal=state.transformNormalObjectToWorld(normal);
	state.ss_normal().set(worldNormal);
	state.ss_geoNormal().set(worldNormal);
	state.setShader(parent->getShader(0));
	state.setModifier(parent->getModifier(0));
	Point3 p=state.transformWorldToObject(state.getPoint());
	float hu,hv;
	switch(k) 
	{
	case 0: 
		{
			hu=p.y;
			hv=p.z;
			break;
		}
	case 1: 
		{
			hu=p.z;
			hv=p.x;
			break;
		}
	case 2: 
		{
			hu=p.x;
			hv=p.y;
			break;
		}
	default:
		hu=hv=0.0f;
	}
	state.ss_tex().x=hu*bnu+hv*bnv+bnd;
	state.ss_tex().y=hu*cnu+hv*cnv+cnd;
	state.setBasis(OrthoNormalBasis::makeFromW(normal));
}

void Plane::intersectPrimitive(Ray&r,int primID,IntersectionState&state)const 
{
	float dn=normal.x*r.dx+normal.y*r.dy+normal.z*r.dz;	
	const float eps=1.0e-6f;
	if(fabs(dn)<eps) return;
	float t=( ((center.x-r.ox)*normal.x) 
		+((center.y-r.oy)*normal.y) 
		+((center.z-r.oz)*normal.z) )/dn; 	
	if(r.isInside(t)) 
	{
		r.setMax(t);		
		state.setIntersection(0);
	}
}

int Plane::getNumPrimitives()const 
{
	return 1;
}

float Plane::getPrimitiveBound(int primID,int i) const
{
	return 0.0f;
}

BoundingBox Plane::getWorldBounds(const Matrix4&o2w)const
{
	return BoundingBox();
}

PrimitiveList* Plane::getBakingPrimitives() const
{
	return NULL;
}