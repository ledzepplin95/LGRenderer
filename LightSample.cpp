#include "StdAfx.h"

#include "LightSample.h"
#include "ShadingState.h"

LightSample::LightSample() 
{
	next=NULL;
}

LightSample::LightSample(const Ray&r,const Color&diff,const Color&spec,
			LightSample*n)
{
    shadowRay=r;
	ldiff=diff;
	lspec=spec;
    next=n;
}

LightSample::LightSample(const LightSample &ls)
{
	if(this==&ls) return;
	shadowRay=ls.shadowRay;
	ldiff=ls.ldiff;
    lspec=ls.lspec;
    next=ls.next;
}

BOOL LightSample::isValid() const
{
	return ldiff!=Color() && lspec!=Color() && !shadowRay.isNull();
}

void LightSample::setShadowRay(const Ray&r) 
{
	shadowRay=r;
}

void LightSample::traceShadow(ShadingState&state) 
{
	Color opacity=state.traceShadow(shadowRay);
	Color::blend(ldiff,Color::BLACK,opacity,ldiff);
	Color::blend(lspec,Color::BLACK,opacity,lspec);
}

Ray LightSample::getShadowRay()const
{
	return shadowRay;
}

Color LightSample::getDiffuseRadiance()const
{
	return ldiff;
}

Color LightSample::getSpecularRadiance() const
{
	return lspec;
}

void LightSample::setRadiance(const Color&d,const Color&s) 
{
	ldiff=d.copy();
	lspec=s.copy();
}

float LightSample::dot(const Vector3&v) const
{
	return shadowRay.dot(v);
}

LightSample& LightSample::operator=(const LightSample&sample)
{
	if(this==&sample) return *this;

	shadowRay=sample.shadowRay;
	ldiff=sample.ldiff;
	lspec=sample.lspec;
	next=sample.next;

	return *this;
}