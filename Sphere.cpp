#include "StdAfx.h"

#include "Sphere.h"
#include "LGdef.h"
#include "solvers.h"
#include "ShadingState.h"
#include "BoundingBox.h"
#include "instance.h"

LG_IMPLEMENT_DYNCREATE(Sphere,PrimitiveList)

BOOL Sphere::update(ParameterList&pl,LGAPI&api)
{
	return TRUE;
}

BoundingBox Sphere::getWorldBounds(const Matrix4&o2w)const 
{
	BoundingBox bounds(1.0f);
	if(o2w!=Matrix4())
	    bounds=o2w.transform(bounds);

	return bounds;
}

float Sphere::getPrimitiveBound(int primID,int i) const
{
	return (i&1)==0?-1.0f:1.0f;
}

int Sphere::getNumPrimitives() const
{
	return 1;
}

void Sphere::prepareShadingState(ShadingState&state) 
{
	state.init();
	state.getRay().getPoint(state.ss_point3());
	Instance* parent=state.getInstance();
	Point3 localPoint=state.transformWorldToObject(state.getPoint());
	state.ss_normal().set(localPoint.x,localPoint.y,localPoint.z);
	state.ss_normal().normalize();

	float phi=(float)atan2(state.getNormal().y,state.getNormal().x);
	if(phi<0.0f)
		phi+=2.0f*LG_PI;
	float theta=(float)acos(state.getNormal().z);
	state.ss_tex().y=theta/(float)LG_PI;
	state.ss_tex().x=phi/(2.0f*LG_PI);
	Vector3 v;
	v.x=-2.0f*LG_PI*state.getNormal().y;
	v.y=2.0f*LG_PI*state.getNormal().x;
	v.z=0.0f;
	state.setShader(parent->getShader(0));
	state.setModifier(parent->getModifier(0));	
	Vector3 worldNormal=state.transformNormalObjectToWorld(state.getNormal());
	v=state.transformVectorObjectToWorld(v);
	state.ss_normal().set(worldNormal);
	state.ss_normal().normalize();
	state.ss_geoNormal().set(state.getNormal());	
	state.setBasis(OrthoNormalBasis::makeFromWV(state.getNormal(),v));
}

void Sphere::intersectPrimitive(Ray&r,int primID,IntersectionState&state)const
{	
	float qa=r.dx*r.dx+r.dy*r.dy+r.dz*r.dz;
	float qb=2.0f*(r.dx*r.ox+r.dy*r.oy+r.dz*r.oz);
	float qc=r.ox*r.ox+r.oy*r.oy+r.oz*r.oz-1.0f;
	vector<double> t=Solvers::solveQuadric(qa,qb,qc);
	if(!t.empty())
	{
		if( t[0]>=r.getMax() || t[1]<=r.getMin() )
			return;
		if( t[0]>r.getMin() )
			r.setMax((float)t[0]);
		else
			r.setMax((float)t[1]);
		state.setIntersection(0);
	}	
}

PrimitiveList* Sphere::getBakingPrimitives()const 
{
	return NULL;
}