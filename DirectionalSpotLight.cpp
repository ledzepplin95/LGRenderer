#include "StdAfx.h"

#include "DirectionalSpotLight.h"
#include "LGdef.h"
#include "ShadingState.h"
#include "parameterlist.h"
#include "Instance.h"

LG_IMPLEMENT_DYNCREATE(DirectionalSpotLight,LightSource)

DirectionalSpotLight::DirectionalSpotLight() 
{
	src.set(0,0,0);
	dir.set(0,0,-1);
	dir.normalize();
	basis=OrthoNormalBasis::makeFromW(dir);
	r=1.0f;
	r2=r*r;
	radiance=Color::WHITE;
}

BOOL DirectionalSpotLight::update(ParameterList&pl,LGAPI&api) 
{
	src=pl.getPoint("source",src);
	dir=pl.getVector("dir",dir);
	dir.normalize();	
	r=pl.getFloat("radius",r);
	basis=OrthoNormalBasis::makeFromW(dir);
	r2=r*r;
	radiance=pl.getColor("radiance",radiance);

	return TRUE;
}

int DirectionalSpotLight::getNumSamples()const
{
	return 1;
}

int DirectionalSpotLight::getLowSamples() const
{
	return 1;
}

void DirectionalSpotLight::getSamples(ShadingState&state)const 
{
	if( Vector3::dot(dir,state.getGeoNormal())<0.0f 
		&& Vector3::dot(dir,state.getNormal())<0.0f )
	{		
		float x=state.getPoint().x-src.x;
		float y=state.getPoint().y-src.y;
		float z=state.getPoint().z-src.z;
		float t=((x*dir.x)+(y*dir.y)+(z*dir.z));
		if( t>=0.0f )
		{
			x-=(t*dir.x);
			y-=(t*dir.y);
			z-=(t*dir.z);
			if( ((x*x)+(y*y)+(z*z))<=r2 ) 
			{
				Point3 p;
				p.x=src.x+x;
				p.y=src.y+y;
				p.z=src.z+z;
				LightSample* dest=new LightSample();
				dest->setShadowRay(Ray(state.getPoint(),p));
				dest->setRadiance(radiance,radiance);
				dest->traceShadow(state);
				state.addSample(dest);
			}
		}
	}
}

void DirectionalSpotLight::getPhoton(double randX1,double randY1, 
	double randX2,double randY2,Point3&p,Vector3&d,Color&power)const
{
	float phi=2.0f*LG_PI*randX1;
	float s=(float)sqrt(1.0f-randY1);
	d.x=r*cos(phi)*s;
	d.y=r*sin(phi)*s;
	d.z=0.0f;
	basis.transform(d);
	Point3::add(src,d,p);
	d.set(dir);
	power.set(radiance).mul((float)LG_PI*r2);
}

float DirectionalSpotLight::getPower()const 
{
	return radiance.copy().mul((float)LG_PI*r2).getLuminance();
}

Instance* DirectionalSpotLight::createInstance() 
{
	return NULL;
}
