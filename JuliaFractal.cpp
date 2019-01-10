#include "StdAfx.h"

#include "JuliaFractal.h"
#include "Solvers.h"
#include "ShadingState.h"
#include "float.h"
#include "parameterlist.h"
#include "boundingbox.h"
#include "instance.h"

float JuliaFractal::BOUNDING_RADIUS=sqrt(3.0);
float JuliaFractal::BOUNDING_RADIUS2=3;
float JuliaFractal::ESCAPE_THRESHOLD=1e1f;
float JuliaFractal::DELTA=1e-4f;

LG_IMPLEMENT_DYNCREATE(JuliaFractal,PrimitiveList)

JuliaFractal::JuliaFractal() 
{	
	cw=-.4f;
	cx=0.2f;
	cy=0.3f;
	cz=-0.2f;

	maxIterations=15;
	epsilon=0.00001f;
}

int JuliaFractal::getNumPrimitives() const
{
	return 1;
}

float JuliaFractal::getPrimitiveBound(int primID,int i) const
{
	return ((i&1)==0)?-BOUNDING_RADIUS:BOUNDING_RADIUS;
}

BoundingBox JuliaFractal::getWorldBounds(const Matrix4&o2w)const 
{
	BoundingBox bounds(BOUNDING_RADIUS);	
    bounds=o2w.transform(bounds);

	return bounds;
}

void JuliaFractal::intersectPrimitive(Ray&r,int primID,IntersectionState&state)const 
{	
	float qc=((r.ox*r.ox)+(r.oy*r.oy)+(r.oz*r.oz))-BOUNDING_RADIUS2;
	float qt=r.getMin();
	if(qc>0.0f)
	{		
		float qa=r.dx*r.dx+r.dy*r.dy+r.dz*r.dz;
		float qb=2.0f*((r.dx*r.ox)+(r.dy*r.oy)+(r.dz*r.oz));
		vector<double> t=Solvers::solveQuadric(qa,qb,qc);		
		if(t.empty() || t[0]>=r.getMax() || t[1]<=r.getMin() )
			return;
		qt=(float)t[0];
	}
	float dist=FLT_MAX;
	float rox=r.ox+qt*r.dx;
	float roy=r.oy+qt*r.dy;
	float roz=r.oz+qt*r.dz;
	float invRayLength=1.0f/sqrt(r.dx*r.dx+r.dy*r.dy+r.dz*r.dz);

	while(TRUE) 
	{
		float zw=rox;
		float zx=roy;
		float zy=roz;
		float zz=0.0f;

		float zpw=1.0f;
		float zpx=0.0f;
		float zpy=0.0f;
		float zpz=0.0f;
	
		float dotz=0.0f;
		for(int i=0; i<maxIterations; i++) 
		{
			{				
				float nw=zw*zpw-zx*zpx-zy*zpy-zz*zpz;
				float nx=zw*zpx+zx*zpw+zy*zpz-zz*zpy;
				float ny=zw*zpy+zy*zpw+zz*zpx-zx*zpz;
				zpz=2*(zw*zpz+zz*zpw+zx*zpy-zy*zpx);
				zpw=2*nw;
				zpx=2*nx;
				zpy=2*ny;
			}
			{				
				float nw=zw*zw-zx*zx-zy*zy-zz*zz+cw;
				zx=2*zw*zx+cx;
				zy=2*zw*zy+cy;
				zz=2*zw*zz+cz;
				zw=nw;
			}
			dotz=zw*zw+zx*zx+zy*zy+zz*zz;
			if(dotz>ESCAPE_THRESHOLD)
				break;
		}
		float normZ=(float)sqrt(dotz);
		dist=0.5f*normZ*(float)log(normZ)/length(zpw,zpx,zpy,zpz);
		rox+=dist*r.dx;
		roy+=dist*r.dy;
		roz+=dist*r.dz;
		qt +=dist;
		if(dist*invRayLength<epsilon)
			break;
		if(rox*rox+roy*roy+roz*roz>BOUNDING_RADIUS2)
			return;
	}	
	if(!r.isInside(qt))
		return;
	if(dist*invRayLength<epsilon)
	{		
		r.setMax(qt);
		state.setIntersection(0);
	}
}

void JuliaFractal::prepareShadingState(ShadingState&state) 
{
	state.init();
	state.getRay().getPoint(state.ss_point3());
	Instance* parent=state.getInstance();
	
	Point3 p=state.transformWorldToObject(state.getPoint());
	float gx1w=p.x-DELTA;
	float gx1x=p.y;
	float gx1y=p.z;
	float gx1z=0.0f;
	float gx2w=p.x+DELTA;
	float gx2x=p.y;
	float gx2y=p.z;
	float gx2z=0.0f;

	float gy1w=p.x;
	float gy1x=p.y-DELTA;
	float gy1y=p.z;
	float gy1z=0.0f;
	float gy2w=p.x;
	float gy2x=p.y+DELTA;
	float gy2y=p.z;
	float gy2z=0.0f;

	float gz1w=p.x;
	float gz1x=p.y;
	float gz1y=p.z-DELTA;
	float gz1z=0.0f;
	float gz2w=p.x;
	float gz2x=p.y;
	float gz2y=p.z+DELTA;
	float gz2z=0.0f;

	for(int i=0; i<maxIterations; i++)
	{
		{			
			float nw=gx1w*gx1w-gx1x*gx1x-gx1y*gx1y-gx1z*gx1z+cw;
			gx1x=2.0*gx1w*gx1x+ cx;
			gx1y=2.0*gx1w*gx1y+cy;
			gx1z=2.0*gx1w*gx1z+cz;
			gx1w=nw;
		}
		{			
			float nw=gx2w*gx2w-gx2x*gx2x-gx2y*gx2y-gx2z*gx2z+cw;
			gx2x=2.0*gx2w*gx2x+cx;
			gx2y=2.0*gx2w*gx2y+cy;
			gx2z=2.0*gx2w*gx2z+cz;
			gx2w=nw;
		}
		{			
			float nw=gy1w*gy1w-gy1x*gy1x-gy1y*gy1y-gy1z*gy1z+cw;
			gy1x=2.0*gy1w*gy1x+cx;
			gy1y=2.0*gy1w*gy1y+cy;
			gy1z=2.0*gy1w*gy1z+cz;
			gy1w=nw;
		}
		{			
			float nw=gy2w*gy2w-gy2x*gy2x-gy2y*gy2y-gy2z*gy2z+cw;
			gy2x=2.0*gy2w*gy2x+cx;
			gy2y=2.0*gy2w*gy2y+cy;
			gy2z=2.0*gy2w*gy2z+cz;
			gy2w=nw;
		}
		{			
			float nw=gz1w*gz1w-gz1x*gz1x-gz1y*gz1y-gz1z*gz1z+cw;
			gz1x=2.0*gz1w*gz1x+cx;
			gz1y=2.0*gz1w*gz1y+cy;
			gz1z=2.0*gz1w*gz1z+cz;
			gz1w=nw;
		}
		{		
			float nw=gz2w*gz2w-gz2x*gz2x-gz2y*gz2y-gz2z*gz2z+cw;
			gz2x=2.0*gz2w*gz2x+cx;
			gz2y=2.0*gz2w*gz2y+cy;
			gz2z=2.0*gz2w*gz2z+cz;
			gz2w=nw;
		}
	}
	float gradX=length(gx2w,gx2x,gx2y,gx2z)-length(gx1w,gx1x,gx1y,gx1z);
	float gradY=length(gy2w,gy2x,gy2y,gy2z)-length(gy1w,gy1x,gy1y,gy1z);
	float gradZ=length(gz2w,gz2x,gz2y,gz2z)-length(gz1w,gz1x,gz1y,gz1z);
	Vector3 n(gradX,gradY,gradZ);
	state.ss_normal().set(state.transformNormalObjectToWorld(n));
	state.ss_normal().normalize();
	state.ss_normal().set(state.getNormal());
	state.setBasis(OrthoNormalBasis::makeFromW(state.getNormal()));

	state.ss_point3().x+=state.getNormal().x*epsilon*20.0f;
	state.ss_point3().y+=state.getNormal().y*epsilon*20.0f;
	state.ss_point3().z+=state.getNormal().z*epsilon*20.0f;

	state.setShader(parent->getShader(0));
	state.setModifier(parent->getModifier(0));
}

float JuliaFractal::length(float w,float x,float y,float z) 
{
	return (float)sqrt(w*w+x*x+y*y+z*z);
}

BOOL JuliaFractal::update(ParameterList&pl,LGAPI&api) 
{
	maxIterations=pl.getInt("iterations",maxIterations);
	epsilon=pl.getFloat("epsilon",epsilon);
	cw=pl.getFloat("cw",cw);
	cx=pl.getFloat("cx",cx);
	cy=pl.getFloat("cy",cy);
	cz=pl.getFloat("cz",cz);

	return TRUE;
}

PrimitiveList* JuliaFractal::getBakingPrimitives() const 
{
	return NULL;
}