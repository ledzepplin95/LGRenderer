#include "StdAfx.h"

#include "PointLight.h"
#include "ShadingState.h"
#include "LGdef.h"
#include "parameterlist.h"
#include "Instance.h"

LG_IMPLEMENT_DYNCREATE(PointLight,LightSource)

PointLight::PointLight() 
{
	lightPoint.set(0,0,0);
	power=Color::WHITE;
}

BOOL PointLight::update(ParameterList&pl,LGAPI&api)
{
	lightPoint=pl.getPoint("center",lightPoint);
	power=pl.getColor("power",power);

	return TRUE;
}

int PointLight::getNumSamples()const 
{
	return 1;
}

void PointLight::getSamples(ShadingState&state)const 
{
	Vector3 d=Point3::sub(lightPoint,state.getPoint(),Vector3());
	if( Vector3::dot(d,state.getNormal())>0.0f
		&& Vector3::dot(d,state.getGeoNormal())>0.0f )
	{
		LightSample *dest=new LightSample();		
		dest->setShadowRay(Ray(state.getPoint(),lightPoint));
		float scale=1.0f/(4.0f*LG_PI*lightPoint.distanceToSquared(state.getPoint()));
		dest->setRadiance(power,power);
		dest->getDiffuseRadiance().mul(scale);
		dest->getSpecularRadiance().mul(scale);
		dest->traceShadow(state);
		state.addSample(dest);
	}
}

void PointLight::getPhoton(double randX1,double randY1,double randX2,double randY2, 
			   Point3&p,Vector3&dir,Color&c)const
{
	p.set(lightPoint);
	float phi=2.0f*LG_PI*randX1;
	float s=(float)sqrt(randY1*(1.0f-randY1));
	dir.x=(float)cos(phi)*s;
	dir.y=(float)sin(phi)*s;
	dir.z=1.0f-2.0f*randY1;
	c.set(power);
}

float PointLight::getPower() const
{
	return power.getLuminance();
}

Instance* PointLight::createInstance()
{
	return NULL;
}