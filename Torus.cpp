#include "StdAfx.h"

#include "Torus.h"
#include "LGdef.h"
#include "Solvers.h"
#include "mathutil.h"
#include "ShadingState.h"
#include "parameterlist.h"
#include "BoundingBox.h"
#include "Instance.h"

LG_IMPLEMENT_DYNCREATE(Torus,PrimitiveList)

Torus::Torus()
{
	ri=0.25f;
	ro=1.0f;
	ri2=ri*ri;
	ro2=ro*ro;
}

BOOL Torus::update(ParameterList&pl,LGAPI&api) 
{
	ri=pl.getFloat("radiusInner",ri);
	ro=pl.getFloat("radiusOuter",ro);
	ri2=ri*ri;
	ro2=ro*ro;

	return TRUE;
}

BoundingBox Torus::getWorldBounds(const Matrix4&o2w)const 
{
	BoundingBox bounds(-ro-ri,-ro-ri,-ri);
	bounds.include(ro+ri,ro+ri,ri);
	bounds=o2w.transform(bounds);

	return bounds;
}

float Torus::getPrimitiveBound(int primID,int i)const
{
	switch(i)
	{
	case 0:
	case 2:
		return -ro-ri;
	case 1:
	case 3:
		return ro+ri;
	case 4:
		return -ri;
	case 5:
		return ri;
	default:
		return 0.0f;
	}
}

int Torus::getNumPrimitives() const
{
	return 1;
}

void Torus::prepareShadingState(ShadingState&state)
{
	state.init();
	state.getRay().getPoint(state.ss_point3());
	Instance* parent=state.getInstance();	
	Point3 p=state.transformWorldToObject(state.getPoint());	
	float deriv=p.x*p.x+p.y*p.y+p.z*p.z-ri2-ro2;
	state.ss_normal().set(p.x*deriv,p.y*deriv,p.z*deriv+2.0f*ro2*p.z);
	state.ss_normal().normalize();

	double phi=asin( MathUtil::clamp((double)p.z/ri,-1.0,1.0) );
	double theta=atan2(p.y,p.x);
	if(theta<0.0f)
		theta+=2.0f*LG_PI;
	state.ss_tex().x=(float)(theta/(2.0f*LG_PI));
	state.ss_tex().y=(float)((phi+LG_PI/2.0)/LG_PI);
	state.setShader(parent->getShader(0));
	state.setModifier(parent->getModifier(0));	
	Vector3 worldNormal=state.transformNormalObjectToWorld(state.getNormal());
	state.ss_normal().set(worldNormal);
	state.ss_normal().normalize();
	state.ss_geoNormal().set(state.getNormal());
	state.setBasis(OrthoNormalBasis::makeFromW(state.getNormal()));
}

void Torus::intersectPrimitive(Ray&r,int primID,IntersectionState&state)const 
{	
	float rd2x=r.dx*r.dx;
	float rd2y=r.dy*r.dy;
	float rd2z=r.dz*r.dz;
	float ro2x=r.ox*r.ox;
	float ro2y=r.oy*r.oy;
	float ro2z=r.oz*r.oz;
	
	double alpha=rd2x+rd2y+rd2z;
	double beta=2.0*(r.ox*r.dx+r.oy*r.dy+r.oz*r.dz);
	double gamma=(ro2x+ro2y+ro2z)-ri2-ro2;

	double A=alpha*alpha;
	double B=2.0*alpha*beta;
	double C=beta*beta+2.0*alpha*gamma+4.0*ro2*rd2z;
	double D=2.0*beta*gamma+8.0*ro2*r.oz*r.dz;
	double E=gamma*gamma+4.0*ro2*ro2z-4.0*ro2*ri2;

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

PrimitiveList* Torus::getBakingPrimitives()const
{
	return NULL;
}