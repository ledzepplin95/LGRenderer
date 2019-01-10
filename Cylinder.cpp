#include "StdAfx.h"
#include "Cylinder.h"

#include "ShadingState.h"
#include "LGdef.h"
#include "Solvers.h"
#include "Instance.h"

LG_IMPLEMENT_DYNCREATE(Cylinder,PrimitiveList)

BOOL Cylinder::update(ParameterList&pl,LGAPI&api)
{
	return TRUE;
}

BoundingBox Cylinder::getWorldBounds(const Matrix4&o2w)const
{
	BoundingBox bounds(1.0f);
	if(o2w!=Matrix4())
	    bounds=o2w.transform(bounds);

	return bounds;
}

float Cylinder::getPrimitiveBound(int primID,int i) const
{
	return (i&1)==0?-1.0f:1.0f;
}

int Cylinder::getNumPrimitives() const
{
	return 1;
}

void Cylinder::prepareShadingState(ShadingState&state) 
{
	state.init();
	state.getRay().getPoint(state.ss_point3());
	Instance* parent=state.getInstance();
	Point3 localPoint=state.transformWorldToObject(state.getPoint());
	state.ss_normal().set(localPoint.x,localPoint.y,0);
	state.ss_normal().normalize();

	float phi=(float)atan2(state.getNormal().y,state.getNormal().x);
	if(phi<0.0)
		phi+=2.0*LG_PI;
	state.ss_tex().x=phi/(2.0f*LG_PI);
	state.ss_tex().y=(localPoint.z+1)*0.5f;
	state.setShader(parent->getShader(0));
	state.setModifier(parent->getModifier(0));
	Vector3 worldNormal=state.transformNormalObjectToWorld(state.getNormal());
	Vector3 v=state.transformVectorObjectToWorld(Vector3(0,0,1));
	state.ss_normal().set(worldNormal);
	state.ss_normal().normalize();
	state.ss_geoNormal().set(state.getNormal());	
	state.setBasis(OrthoNormalBasis::makeFromWV(state.getNormal(),v));
}

void Cylinder::intersectPrimitive(Ray&r,int primID,IntersectionState&state)const
{	
	float qa=r.dx*r.dx+r.dy*r.dy;
	float qb=2.0f*((r.dx*r.ox)+(r.dy*r.oy));
	float qc=r.ox*r.ox+r.oy*r.oy-1.0f;
	vector<double> t=Solvers::solveQuadric(qa,qb,qc);			
	if( t[0]>=r.getMax() || t[1]<=r.getMin() )
		return;
	if( t[0]>r.getMin() ) 
	{
		float z=r.oz+(float)t[0]*r.dz;
		if( z>=-1.0f && z<=1.0f )
		{
			r.setMax((float)t[0]);
			state.setIntersection(0);
			return;
		}
	}
	if( t[1]<r.getMax() ) 
	{
		float z=r.oz+(float)t[1]*r.dz;
		if( z>=-1.0f && z<=1.0f )
		{
			r.setMax((float)t[1]);
			state.setIntersection(0);
		}
	}	
}

PrimitiveList* Cylinder::getBakingPrimitives()const 
{
	return NULL;
}