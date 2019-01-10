#include "StdAfx.h"

#include "PerlinModifier.h"
#include "ShadingState.h"
#include "PerlinScalar.h"
#include "LGdef.h"
#include "parameterlist.h"

LG_IMPLEMENT_DYNCREATE(PerlinModifier,Modifier)

PerlinModifier::PerlinModifier()
{
	_function=0;
	_scale=50.0f;
    _size=1.0f;
}

BOOL PerlinModifier::update(ParameterList&pl,LGAPI&api)
{
	_function=pl.getInt("function",_function);
	_size=pl.getFloat("size",_size);
	_scale=pl.getFloat("scale",_scale);

	return TRUE;
}

void PerlinModifier::modify(ShadingState&state)const 
{
	Point3 p=state.transformWorldToObject(state.getPoint());
	p.x*=_size;
	p.y*=_size;
	p.z*=_size;
	Vector3 normal=state.transformNormalWorldToObject(state.getNormal());
	double f0=f(p.x, p.y,p.z);
	double fx=f(p.x+.0001, p.y,p.z);
	double fy=f(p.x,p.y+.0001,p.z);
	double fz=f(p.x,p.y,p.z + .0001);

	normal.x-=_scale*(fx-f0)/.0001;
	normal.y-=_scale*(fy-f0)/.0001;
	normal.z-=_scale*(fz-f0)/.0001;
	normal.normalize();

	state.ss_normal().set(state.transformNormalObjectToWorld(normal));
	state.ss_normal().normalize();
	state.setBasis(OrthoNormalBasis::makeFromW(state.getNormal()));
}

double PerlinModifier::f(double x,double y,double z) const
{
	switch(_function) 
	{
	case 0:
		return .03*noise(x,y,z,8);
	case 1:
		return .01*stripes(x+2*turbulence(x,y,z,1),1.6);
	default:
		return -.10*turbulence(x,y,z,1);
	}
}

double PerlinModifier::stripes(double x,double f)
{
	double t=0.5+0.5*sin(f*2.0*LG_PI*x);

	return t*t-.5;
}

double PerlinModifier::turbulence(double x,double y,double z,double freq) 
{
	double t=-.5;
	for(; freq<=300/12; freq*=2)
		t+= fabs(noise(x,y,z,freq)/freq);

	return t;
}

 double PerlinModifier::noise(double x,double y,double z,double freq) 
{
	double x1,y1,z1;
	x1=.707*x-.707*z;
	z1=.707*x+.707*z;
	y1=.707*x1+.707*y;
	x1=.707*x1-.707*y;

	return PerlinScalar::snoise((float)(freq*x1+100.0f),
		(float)(freq*y1),(float)(freq*z1));
}