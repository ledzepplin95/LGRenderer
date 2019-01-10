#include "StdAfx.h"

#include "AnisotropicWardShader.h"
#include "LGDef.h"
#include "ShadingState.h"
#include "parameterlist.h"

LG_IMPLEMENT_DYNCREATE(AnisotropicWardShader,Shader)

AnisotropicWardShader::AnisotropicWardShader() 
{
	rhoD=Color::GRAY;
	rhoS=Color::GRAY;
	alphaX=1;
	alphaY=1;
	numRays=4;
}

BOOL AnisotropicWardShader::update(ParameterList&pl,LGAPI&api)
{
	rhoD=pl.getColor("diffuse",rhoD);
	rhoS=pl.getColor("specular",rhoS);
	alphaX=pl.getFloat("roughnessX",alphaX);
	alphaY=pl.getFloat("roughnessY",alphaY);
	numRays=pl.getInt("samples",numRays);

	return TRUE;
}

Color AnisotropicWardShader::getDiffuse(ShadingState&state) const
{
	return rhoD;
}

float AnisotropicWardShader::brdf(const Vector3&i,const Vector3&o,const OrthoNormalBasis&basis) const
{
	float fr=4.0f*LG_PI*alphaX*alphaY;
	float p=basis.untransformZ(i)*basis.untransformZ(o);
	if(p>0.0f)
		fr*=(float)sqrt(p);
	else
		fr=0.0;
	Vector3 h=Vector3::add(i,o,Vector3());
	basis.untransform(h);
	float hx=h.x/alphaX;
	hx*=hx;
	float hy=h.y/alphaY;
	hy*=hy;
	float hn=h.z*h.z;
	if(fr>0.0f)
		fr=(float)exp(-(hx+hy)/hn)/fr;

	return fr;
}

Color AnisotropicWardShader::getRadiance(ShadingState&state)const 
{	
	state.faceforward();
	OrthoNormalBasis onb=state.getBasis();	
	state.initLightSamples();
	state.initCausticSamples();
	Color lr=Color::BLACK;	
	if(state.includeSpecular()) 
	{
		Vector3 inside=state.getRay().getDirection().negate(Vector3());
	    for(LightSample*sample=state.ss_lightSampleHead(); sample!=NULL; sample=sample->next)
	   {
			float cosNL=sample->dot(state.getNormal());
			float fr=brdf(inside,sample->getShadowRay().getDirection(),onb);
			lr.madd(cosNL*fr,sample->getSpecularRadiance());
		}
		
		if(numRays>0) 
		{
			int n=state.getDepth()==0?numRays:1;
			for(int i=0; i<n; i++) 
			{			
				double r1=state.getRandom(i,0,n);
				double r2=state.getRandom(i,1,n);

				float alphaRatio=alphaY/alphaX;
				float phi=0.0f;
				if(r1<0.25) 
				{
					double val=4.0*r1;
					phi=(float)atan(alphaRatio*tan(LG_PI/2.0*val));
				}
				else if(r1<0.5) 
				{
					double val=1.0-4.0*(0.5-r1);
					phi=(float)atan(alphaRatio*tan(LG_PI/2.0*val));
					phi=(float)LG_PI-phi;
				} 
				else if(r1<0.75) 
				{
					double val=4.0*(r1-0.5);
					phi=(float)atan(alphaRatio*tan(LG_PI/2.0*val));
					phi+=LG_PI;
				} 
				else 
				{
					double val=1.0-4.0*(1.0-r1);
					phi=(float) atan(alphaRatio*tan(LG_PI/2.0*val));
					phi=2.0f*(float)LG_PI-phi;
				}

				float cosPhi=(float)cos(phi);
				float sinPhi=(float)sin(phi);

				float denom=(cosPhi*cosPhi)/(alphaX*alphaX) 
					+(sinPhi*sinPhi)/(alphaY*alphaY);
				float theta=(float)atan(sqrt(-log(1.0-r2)/denom));

				float sinTheta=(float)sin(theta);
				float cosTheta=(float)cos(theta);

				Vector3 h;
				h.x=sinTheta*cosPhi;
				h.y=sinTheta*sinPhi;
				h.z=cosTheta;
				onb.transform(h);

				Vector3 o;
				float ih=Vector3::dot(h,inside);
				o.x=2.0*ih*h.x-inside.x;
				o.y=2.0*ih*h.y-inside.y;
				o.z=2.0*ih*h.z-inside.z;

				float no=onb.untransformZ(o);
				float ni=onb.untransformZ(inside);
				float w=ih*cosTheta*cosTheta*cosTheta
					*(float)sqrt(fabs(no/ni));

				Ray r(state.getPoint(),o);
				lr.madd(w/n,state.traceGlossy(r,i));
			}
		}
		lr.mul(rhoS);
	}	
	lr.add(state.diffuse(getDiffuse(state)));

	return lr;
}

void AnisotropicWardShader::scatterPhoton(ShadingState&state,Color&power)const
{
	state.faceforward();
	Color d=getDiffuse(state);
	state.storePhoton(state.getRay().getDirection(),power,d);
	float avgD=d.getAverage();
	float avgS=rhoS.getAverage();
	double rnd=state.getRandom(0, 0, 1);
	if(rnd<avgD) 
	{		
		power.mul(d).mul(1.0f/avgD);
		OrthoNormalBasis onb=state.getBasis();
		double u=2.0*LG_PI*rnd/avgD;
		double v=state.getRandom(0,1,1);
		float s=(float)sqrt(v);
		float s1=(float)sqrt(1.0f-v);
		Vector3 w((float)cos(u)*s,(float)sin(u)*s,s1);
		w=onb.transform(w,Vector3());
		state.traceDiffusePhoton(Ray(state.getPoint(),w),power);
	} 
	else if(rnd<avgD+avgS) 
	{		
		power.mul(rhoS).mul(1.0/avgS);
		OrthoNormalBasis basis=state.getBasis();
		Vector3 inside=state.getRay().getDirection().negate(Vector3());
		double r1=rnd/avgS;
		double r2=state.getRandom(0,1,1);

		float alphaRatio=alphaY/alphaX;
		float phi=0.0f;
		if(r1<0.25)
		{
			double val=4.0*r1;
			phi=(float)atan(alphaRatio*tan(LG_PI/2.0*val));
		} 
		else if(r1<0.5)
		{
			double val=1.0-4.0*(0.5-r1);
			phi=(float)atan(alphaRatio*tan(LG_PI/2.0*val));
			phi=(float)LG_PI-phi;
		} 
		else if(r1<0.75) 
		{
			double val=4.0*(r1-0.5);
			phi=(float)atan(alphaRatio*tan(LG_PI/2.0*val));
			phi+=LG_PI;
		} 
		else 
		{
			double val=1.0-4.0*(1.0-r1);
			phi=(float)atan(alphaRatio*tan(LG_PI/2.0*val));
			phi=2.0*(float)LG_PI-phi;
		}

		float cosPhi=(float)cos(phi);
		float sinPhi=(float)sin(phi);

		float denom=(cosPhi*cosPhi)/(alphaX*alphaX)
			+(sinPhi*sinPhi)/(alphaY*alphaY);
		float theta=(float)atan(sqrt(-log(1.0-r2)/denom));

		float sinTheta=(float)sin(theta);
		float cosTheta=(float)cos(theta);

		Vector3 h;
		h.x=sinTheta*cosPhi;
		h.y=sinTheta*sinPhi;
		h.z=cosTheta;
		basis.transform(h);

		Vector3 o;
		float ih=Vector3::dot(h,inside);
		o.x=2.0*ih*h.x-inside.x;
		o.y=2.0*ih*h.y-inside.y;
		o.z=2.0*ih*h.z-inside.z;

		Ray r(state.getPoint(),o);
		state.traceReflectionPhoton(r,power);
	}
}