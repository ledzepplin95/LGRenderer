#include "StdAfx.h"

#include "CornellBox.h"
#include "ShadingState.h"
#include "float.h"
#include "LGdef.h"
#include "parameterlist.h"
#include "instance.h"

LG_IMPLEMENT_DYNCREATE(CornellBox,PrimitiveList)

CornellBox::CornellBox()
{
	updateGeometry(Point3(-1,-1,-1),Point3(1,1,1));
	
	left.set(0.80f,0.25f,0.25f);
	right.set(0.25f,0.25f,0.80f);
	Color gray(0.70f,0.70f,0.70f);
	top=bottom=back=gray;
	
	radiance=Color::WHITE;
	samples=16;
}

void CornellBox::updateGeometry(const Point3&c0,const Point3&c1)
{	
	lightBounds.set(c0);
	lightBounds.include(c1);

	minX=lightBounds.getMinimum().x;
	minY=lightBounds.getMinimum().y;
	minZ=lightBounds.getMinimum().z;
	maxX=lightBounds.getMaximum().x;
	maxY=lightBounds.getMaximum().y;
	maxZ=lightBounds.getMaximum().z;
	
	lightBounds.enlargeUlps();
	
	lxmin=maxX/3+2*minX/3;
	lxmax=minX/3+2*maxX/3;
	lymin=maxY/3+2*minY/3;
	lymax=minY/3+2*maxY/3;
	area=(lxmax-lxmin)*(lymax-lymin);
}

BOOL CornellBox::update(ParameterList&pl,LGAPI&api) 
{
	PrimitiveList::update(pl,api);
	Shader::update(pl,api);
	LightSource::update(pl,api);
	Point3 corner0=pl.getPoint("corner0",Point3());
	Point3 corner1=pl.getPoint("corner1",Point3());
	if(corner0!=Point3() && corner1!=Point3())
	    updateGeometry(corner0,corner1);	
	
	left=pl.getColor("leftColor",left);
	right=pl.getColor("rightColor",right);
	top=pl.getColor("topColor",top);
	bottom=pl.getColor("bottomColor",bottom);
	back=pl.getColor("backColor",back);

	radiance=pl.getColor("radiance",radiance);
	samples=pl.getInt("samples",samples);

	return TRUE;
}

BoundingBox CornellBox::getBounds()const 
{
	return lightBounds;
}

float CornellBox::getBound(int i) const 
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

BOOL CornellBox::intersects(const BoundingBox&box) 
{	
	BoundingBox b;
	b.include(Point3(minX,minY,minZ));
	b.include(Point3(maxX,maxY,maxZ));
	if(b.intersects(box)) 
	{		
		if(!b.contains(Point3(box.getMinimum().x,box.getMinimum().y,box.getMinimum().z)))
			return TRUE;
		if(!b.contains(Point3(box.getMinimum().x,box.getMinimum().y,box.getMaximum().z)))
			return TRUE;
		if(!b.contains(Point3(box.getMinimum().x,box.getMaximum().y,box.getMinimum().z)))
			return TRUE;
		if(!b.contains(Point3(box.getMinimum().x,box.getMaximum().y,box.getMaximum().z)))
			return TRUE;
		if(!b.contains(Point3(box.getMaximum().x,box.getMinimum().y,box.getMinimum().z)))
			return TRUE;
		if(!b.contains(Point3(box.getMaximum().x,box.getMinimum().y,box.getMaximum().z)))
			return TRUE;
		if(!b.contains(Point3(box.getMaximum().x,box.getMaximum().y,box.getMinimum().z)))
			return TRUE;
		if(!b.contains(Point3(box.getMaximum().x,box.getMaximum().y,box.getMaximum().z)))
			return TRUE;		
	}

	return FALSE;
}

void CornellBox::prepareShadingState(ShadingState&state)
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
	state.setShader(this);
}

void CornellBox::intersectPrimitive(Ray&r,int primID,IntersectionState&state)const 
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
	ASSERT(sideIn!=-1);
	ASSERT(sideOut!=-1);
	
	if( sideIn!=2 && r.isInside(intervalMin) ) 
	{
		r.setMax(intervalMin);
		state.setIntersection(sideIn);
	} 
	else if( sideOut!=2 && r.isInside(intervalMax) )
	{
		r.setMax(intervalMax);
		state.setIntersection(sideOut);
	}
}

Color CornellBox::getRadiance(ShadingState&state)const
{
	int side=state.getPrimitiveID();
	Color kd;
	switch(side) 
	{
	case 0:
		kd=left;
		break;
	case 1:
		kd=right;
		break;
	case 3:
		kd=back;
		break;
	case 4:
		kd=bottom;
		break;
	case 5:
		{
			float lx=state.getPoint().x;
			float ly=state.getPoint().y;
			if( lx>=lxmin && lx<lxmax && ly>=lymin 
				&& ly<lymax && state.getRay().dz>0.0f )
				return state.includeLights()?radiance:Color::BLACK;
			kd=top;
		}				
		break;
	default:
		ASSERT(FALSE);
	}	
	state.faceforward();
	state.initLightSamples();
	state.initCausticSamples();	

	return state.diffuse(kd);
}

void CornellBox::scatterPhoton(ShadingState&state,Color&power)const 
{
	int side=state.getPrimitiveID();
	Color kd;
	switch(side) 
	{
	case 0:
		kd=left;
		break;
	case 1:
		kd=right;
		break;
	case 3:
		kd=back;
		break;
	case 4:
		kd=bottom;
		break;
	case 5:
		{
			float lx=state.getPoint().x;
			float ly=state.getPoint().y;
			if( lx>=lxmin && lx<lxmax && ly>=lymin 
				&& ly<lymax && state.getRay().dz>0.0f )
				return;
			kd=top;
		}				
		break;
	default:
		ASSERT(FALSE);
	}

	if( Vector3::dot(state.getNormal(),state.getRay().getDirection())>0.0f ) 
	{
		state.ss_normal().negate();
		state.ss_geoNormal().negate();
	}
	state.storePhoton(state.getRay().getDirection(),power,kd);
	double avg=kd.getAverage();
	double rnd=state.getRandom(0,0,1);
	if(rnd<avg) 
	{		
		power.mul(kd).mul(1.0f/(float)avg);
		OrthoNormalBasis onb=OrthoNormalBasis::makeFromW(state.getNormal());
		double u=2.0*LG_PI*rnd/avg;
		double v=state.getRandom(0,1,1);
		float s=(float)sqrt(v);
		float s1=(float)sqrt(1.0-v);
		Vector3 w((float)cos(u)*s,(float)sin(u)*s,s1);
		w=onb.transform(w,Vector3());
		state.traceDiffusePhoton(Ray(state.getPoint(),w),power);
	}
}

int CornellBox::getNumSamples() const 
{
	return samples;
}

void CornellBox::getSamples(ShadingState&state)const  
{
	if( lightBounds.contains(state.getPoint()) && state.getPoint().z<maxZ ) 
	{
		int n=state.getDiffuseDepth()>0?1:samples;
		float a=area/n;
		for(int i=0; i<n; i++)
		{			
			double randX=state.getRandom(i,0,n);
			double randY=state.getRandom(i,1,n);
			Point3 p;
			p.x=(float)(lxmin*(1.0f-randX)+lxmax*randX);
			p.y=(float)(lymin*(1.0f-randY)+lymax*randY);
			p.z=maxZ-0.001f;

			LightSample *dest=new LightSample();			
			dest->setShadowRay(Ray(state.getPoint(),p));
			
			float cosNx=dest->dot(state.getNormal());
			if(cosNx<=0.0f)
			{
				delete dest;
				return;
			}
			
			float cosNy=dest->getShadowRay().dz;
			if(cosNy>0.0f) 
			{				
				float r=dest->getShadowRay().getMax();
				float g=cosNy/(r*r);
				float scale=g*a;
				
				dest->setRadiance(radiance,radiance);				
				dest->ldiff.mul(scale);
				dest->lspec.mul(scale);
				dest->traceShadow(state);
				state.addSample(dest);
			}
			else 
			{
				delete dest;
			}
		}
	}
}

void CornellBox::getPhoton(double randX1,double randY1,double randX2,double randY2,
			   Point3&p,Vector3&dir,Color&power) const
{
	p.x=(float)(lxmin*(1.0f-randX2)+lxmax*randX2);
	p.y=(float)(lymin*(1.0f-randY2)+lymax*randY2);
	p.z=maxZ-0.001f;

	double u=2.0*LG_PI*randX1;
	double s=sqrt(randY1);
	dir.set((float)(cos(u)*s),(float)(sin(u)*s),(float)-sqrt(1.0f-randY1));
	Color::mul((float)LG_PI*area,radiance,power);
}

float CornellBox::getPower()const 
{
	return radiance.copy().mul((float)LG_PI*area).getLuminance();
}

int CornellBox::getNumPrimitives() const
{
	return 1;
}

float CornellBox::getPrimitiveBound(int primID,int i)const 
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

BoundingBox CornellBox::getWorldBounds(const Matrix4&o2w) const
{
	BoundingBox bounds(minX,minY,minZ);
	bounds.include(maxX,maxY,maxZ);
	if(o2w==Matrix4()) return bounds;
	
	return o2w.transform(bounds);
}

PrimitiveList* CornellBox::getBakingPrimitives() const
{
	return NULL;
}

Instance* CornellBox::createInstance() 
{
	return Instance::createTemporary(*this,Matrix4(),this);
}