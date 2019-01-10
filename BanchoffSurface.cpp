#include "StdAfx.h"

#include "BanchoffSurface.h"
#include "Solvers.h"
#include "ShadingState.h"
#include "OrthoNormalBasis.h"
#include "BoundingBox.h"
#include "instance.h"

LG_IMPLEMENT_DYNCREATE(BanchoffSurface,PrimitiveList)

BOOL BanchoffSurface::update(ParameterList&pl,LGAPI&api) 
{
	return TRUE;
}

BoundingBox BanchoffSurface::getWorldBounds(const Matrix4&o2w)const 
{
	BoundingBox bounds(1.5f);	
	bounds=o2w.transform(bounds);

	return bounds;
}

float BanchoffSurface::getPrimitiveBound(int primID,int i)const
{
	return (i&1)==0?-1.5f:1.5f;
}

int BanchoffSurface::getNumPrimitives() const
{
	return 1;
}

void BanchoffSurface::prepareShadingState(ShadingState&state) 
{
	state.init();
	state.getRay().getPoint(state.ss_point3());
	Instance* parent=state.getInstance();
	Point3 n=state.transformWorldToObject(state.getPoint());
	state.ss_normal().set(n.x*(2.0f*n.x*n.x-1.0f),
		n.y*(2.0f*n.y*n.y-1.0f),n.z*(2.0f*n.z*n.z-1.0f));
	state.ss_normal().normalize();
	state.setShader(parent->getShader(0));
	state.setModifier(parent->getModifier(0));

	Vector3 worldNormal=state.transformNormalObjectToWorld(state.getNormal());
	state.ss_normal().set(worldNormal);
	state.ss_normal().normalize();
	state.ss_geoNormal().set(state.getNormal());
	state.setBasis(OrthoNormalBasis::makeFromW(state.getNormal()));
}

void BanchoffSurface::intersectPrimitive(Ray&r,int primID,IntersectionState&state)const 
{
	float rd2x=r.dx*r.dx;
	float rd2y=r.dy*r.dy;
	float rd2z=r.dz*r.dz;
	float ro2x=r.ox*r.ox;
	float ro2y=r.oy*r.oy;
	float ro2z=r.oz*r.oz;
	
	double A=(rd2y*rd2y+rd2z*rd2z+rd2x*rd2x);
	double B=4.0*(r.oy*rd2y*r.dy+r.oz*r.dz*rd2z+r.ox*r.dx*rd2x);
	double C=(-rd2x-rd2y-rd2z+6.0*(ro2y*rd2y+ro2z*rd2z+ro2x*rd2x));
	double D=2.0*(2.0*ro2z*r.oz*r.dz-r.oz*r.dz+2.0*ro2x*r.ox*r.dx+2.0*ro2y*r.oy*r.dy-r.ox*r.dx-r.oy*r.dy);
	double E=3.0/8.0+(-ro2z+ro2z*ro2z-ro2y+ro2y*ro2y-ro2x+ro2x*ro2x);

	vector<double> t=Solvers::solveQuartic(A,B,C,D,E);
	if(!t.empty()) 
	{	
		if( t[0]>=r.getMax() || t[t.size()-1]<=r.getMin() )
			return;		
		for(int i=0; i<t.size(); i++)
		{
			if(t[i]>r.getMin()) 
			{
				r.setMax((float)t[i]);
				state.setIntersection(0);

				return;
			}
		}
	}
}

PrimitiveList* BanchoffSurface::getBakingPrimitives()const 
{
	return NULL;
}