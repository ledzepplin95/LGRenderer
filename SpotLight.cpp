#include "StdAfx.h"
#include "SpotLight.h"
#include "lgdef.h"
#include "ParameterList.h"
#include "ShadingState.h"

LG_IMPLEMENT_DYNCREATE(SpotLight,LightSource)

SpotLight::SpotLight(void)
{
	position.set(0,0,0);
	direction.set(0,0,-1);
	intensity.set(1,1,1);
    minAngle=LG_PI/2.0;
	maxAngle=LG_PI/4.0;
}

BOOL SpotLight::update(ParameterList &pl,LGAPI&api)
{
    position=pl.getPoint("position",position);
	direction=pl.getVector("dir",direction);
    intensity=pl.getColor("intensity",intensity);
	minAngle=pl.getFloat("minAngle",minAngle);
	maxAngle=pl.getFloat("maxAngle",maxAngle);

	return TRUE;
}

int SpotLight::getNumSamples() const
{
	return 1;
}

void SpotLight::getSamples(ShadingState&state) const
{
	Vector3 dir=state.getPoint()-position;
	double t=dir.length();
	dir.normalize();
    double cosAngle=dir.dot(direction.x,direction.y,direction.z);
	if(cosAngle<0.0) return;
	float angle=acos(cosAngle);
    state.ss_ray().reverseDirection();
    state.ss_ray().setMax(t);
	float falloff=1.0f;
	if(angle>minAngle)
	{
		falloff=(maxAngle-angle)/(maxAngle-minAngle);
	}

	Color c;
	const float epsilon=1.0e-3;
	c=Color::mul(1.0f/(t+epsilon),intensity);
	c=Color::mul(falloff,c);
	LightSample *dest=new LightSample();		
	dest->setShadowRay(Ray(state.getPoint(),position));
	float scale=1.0f/(4.0f*LG_PI*position.distanceToSquared(state.getPoint()));
	dest->setRadiance(c,c);
	dest->getDiffuseRadiance().mul(scale);
	dest->getSpecularRadiance().mul(scale);
	dest->traceShadow(state);
	state.addSample(dest);
}

void SpotLight::getPhoton(double randX1,double randY1,double randX2,double randY2, 
						  Point3 &p,Vector3 &dir,Color &c) const
{
	p.set(position);
	float phi=2.0f*LG_PI*randX1;
	float s=(float)sqrt(randY1*(1.0f-randY1));
	dir.x=(float)cos(phi)*s;
	dir.y=(float)sin(phi)*s;
	dir.z=1.0f-2.0f*randY1;
	c.set(intensity);
}

float SpotLight::getPower() const
{
	return intensity.getLuminance();
}

Instance* SpotLight::createInstance()
{
	return NULL;
}