#include "StdAfx.h"

#include "SphereLight.h"
#include "LGdef.h"
#include "ShadingState.h"
#include "Solvers.h"
#include "Sphere.h"
#include "parameterlist.h"
#include "Instance.h"

LG_IMPLEMENT_DYNCREATE(SphereLight,LightSource)

SphereLight::SphereLight() 
{
	radiance=Color::WHITE;
	numSamples=4;	
	radius=r2=1.0f;
}

BOOL SphereLight::update(ParameterList&pl,LGAPI&api) 
{
	LightSource::update(pl,api);
	Shader::update(pl,api);
	radiance=pl.getColor("radiance",radiance);
	numSamples=pl.getInt("samples",numSamples);
	radius=pl.getFloat("radius",radius);
	r2=radius*radius;
	center=pl.getPoint("center",center);

	return TRUE;
}

int SphereLight::getNumSamples() const
{
	return numSamples;
}

int SphereLight::getLowSamples() const
{
	return 1;
}

BOOL SphereLight::isVisible(ShadingState&state) const
{
	return state.getPoint().distanceToSquared(center)>r2;
}

void SphereLight::getSamples(ShadingState&state)const 
{
	if(getNumSamples()<=0) return;
	Vector3 wc=Point3::sub(center,state.getPoint(),Vector3());
	float l2=wc.lengthSquared();
	if(l2<=r2) return;

	float topX=wc.x+state.getNormal().x*radius;
	float topY=wc.y+state.getNormal().y*radius;
	float topZ=wc.z+state.getNormal().z*radius;
	if( state.getNormal().dot(topX,topY,topZ)<=0.0f )
		return; 
	float cosThetaMax=(float)sqrt(max(0,1.0f-r2/Vector3::dot(wc,wc)));
	OrthoNormalBasis basis=OrthoNormalBasis::makeFromW(wc);
	int samples=state.getDiffuseDepth()>0?1:getNumSamples();
	float scale=2.0f*LG_PI*(1.0f-cosThetaMax);
	Color c=Color::mul(scale/samples,radiance);
	for(int i=0; i<samples; i++) 
	{		
		double randX=state.getRandom(i,0,samples);
		double randY=state.getRandom(i,1,samples);	
		double cosTheta=(1.0-randX)*cosThetaMax+randX;
		double sinTheta=sqrt(1.0-cosTheta*cosTheta);
		double phi=randY*2.0*LG_PI;
		Vector3 dir((float)(cos(phi)*sinTheta), 
			(float)(sin(phi)*sinTheta),(float)cosTheta);
		basis.transform(dir);
		
		float cosNx=Vector3::dot(dir,state.getNormal());
		if( cosNx<=0.0f ) continue;

		float ocx=state.getPoint().x-center.x;
		float ocy=state.getPoint().y-center.y;
		float ocz=state.getPoint().z-center.z;
		float qa=Vector3::dot(dir,dir);
		float qb=2.0f*((dir.x*ocx)+(dir.y*ocy)+(dir.z*ocz));
		float qc=((ocx*ocx)+(ocy*ocy)+(ocz*ocz))-r2;
		vector<double> t=Solvers::solveQuadric(qa,qb,qc);
		if(t.empty()) continue;
		LightSample* dest=new LightSample();	
		dest->setShadowRay(Ray(state.getPoint(),dir));		
		dest->getShadowRay().setMax((float)t[0]-1e-3f);		
		dest->setRadiance(c,c);
		dest->traceShadow(state);
		state.addSample(dest);
	}
}

void SphereLight::getPhoton(double randX1,double randY1,double randX2,double randY2,
			   Point3&p,Vector3&dir,Color&power)const
{
	float z=1.0f-2.0*randX2;
	float r=(float)sqrt(max(0,1-z*z));
	float phi=2.0f*LG_PI*randY2;
	float x=r*cos(phi);
	float y=r*sin(phi);
	p.x=center.x+x*radius;
	p.y=center.y+y*radius;
	p.z=center.z+z*radius;
	OrthoNormalBasis basis=OrthoNormalBasis::makeFromW(Vector3(x,y,z));
	phi=2.0f*LG_PI*randX1;
	float cosPhi=(float)cos(phi);
	float sinPhi=(float)sin(phi);
	float sinTheta=(float)sqrt(randY1);
	float cosTheta=(float)sqrt(1.0-randY1);
	dir.x=cosPhi*sinTheta;
	dir.y=sinPhi*sinTheta;
	dir.z=cosTheta;
	basis.transform(dir);
	power.set(radiance);
	power.mul(4.0f*LG_PI*LG_PI*r2);
}

float SphereLight::getPower()const
{
	return radiance.copy().mul(4.0f*LG_PI*LG_PI*r2).getLuminance();
}

Color SphereLight::getRadiance(ShadingState&state)const 
{
	if(!state.includeLights())
		return Color::BLACK;
	state.faceforward();

	return state.isBehind()?Color::BLACK:radiance;
}

void SphereLight::scatterPhoton(ShadingState&state,Color&power)const 
{	
}

Instance* SphereLight::createInstance() 
{
	return Instance::createTemporary(Sphere(),Matrix4::translation(center.x,
		center.y,center.z).multiply(Matrix4::scale(radius)),this);	
}
