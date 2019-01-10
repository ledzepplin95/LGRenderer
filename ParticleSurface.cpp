#include "StdAfx.h"

#include "ParticleSurface.h"
#include "solvers.h"
#include "ShadingState.h"
#include "parameterlist.h"
#include "boundingbox.h"
#include "instance.h"

LG_IMPLEMENT_DYNCREATE(ParticleSurface,PrimitiveList)

ParticleSurface::ParticleSurface() 
{	
	r=r2=1.0f;
	n=0;
}

int ParticleSurface::getNumPrimitives() const
{
	return n;
}

float ParticleSurface::getPrimitiveBound(int primID,int i)const
{
	float c=particles[primID*3+(i>>1)];

	return (i&1)==0?c-r:c+r;
}

BoundingBox ParticleSurface::getWorldBounds(const Matrix4&o2w)const
{
	BoundingBox bounds(0.0f);
	for(int i=0, i3=0; i<n; i++, i3+=3)
		bounds.include(particles[i3], particles[i3+1],particles[i3+2]);
	bounds.include(bounds.getMinimum().x-r,bounds.getMinimum().y-r,bounds.getMinimum().z-r);
	bounds.include(bounds.getMaximum().x+r,bounds.getMaximum().y+r,bounds.getMaximum().z+r);

	return o2w.transform(bounds);
}

void ParticleSurface::intersectPrimitive(Ray&r,int primID,IntersectionState&state) const
{
	int i3=primID*3;
	float ocx=r.ox-particles[i3+0];
	float ocy=r.oy-particles[i3+1];
	float ocz=r.oz-particles[i3+2];
	float qa=r.dx*r.dx+r.dy*r.dy+r.dz*r.dz;
	float qb=2.0f*((r.dx*ocx)+(r.dy*ocy)+(r.dz*ocz));
	float qc=((ocx*ocx)+(ocy*ocy)+(ocz*ocz))-r2;
	vector<double> t=Solvers::solveQuadric(qa,qb,qc);
	if(!t.empty())
	{		
		if( t[0]>=r.getMax() || t[1]<=r.getMin() )
			return;
		if( t[0]>r.getMin() )
			r.setMax((float)t[0]);
		else
			r.setMax((float)t[1]);
		state.setIntersection(primID);
	}
}

void ParticleSurface::prepareShadingState(ShadingState&state)
{
	state.init();
	state.getRay().getPoint(state.ss_point3());
	Point3 localPoint=state.transformWorldToObject(state.getPoint());

	localPoint.x-=particles[3*state.getPrimitiveID()+0];
	localPoint.y-=particles[3*state.getPrimitiveID()+1];
	localPoint.z-=particles[3*state.getPrimitiveID()+2];

	state.ss_normal().set(localPoint.x,localPoint.y,localPoint.z);
	state.ss_normal().normalize();

	state.setShader(state.getInstance()->getShader(0));
	state.setModifier(state.getInstance()->getModifier(0));
	Vector3 worldNormal=state.transformNormalObjectToWorld(state.getNormal());
	state.ss_normal().set(worldNormal);
	state.ss_normal().normalize();
	state.ss_geoNormal().set(state.getNormal());
	state.setBasis(OrthoNormalBasis::makeFromW(state.getNormal()));
}

BOOL ParticleSurface::update(ParameterList&pl,LGAPI&api) 
{
	ParameterList::FloatParameter p=pl.getPointArray("particles");
	particles=p.data;
	r=pl.getFloat("radius",r);
	r2=r*r;
	n=pl.getInt("num",n);

	return !particles.empty() && n<=(particles.size()/3);
}

PrimitiveList* ParticleSurface::getBakingPrimitives() const
{
	return NULL;
}
