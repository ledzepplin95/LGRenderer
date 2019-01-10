#include "StdAfx.h"

#include "PhongShader.h"
#include "ShadingState.h"
#include "LGDef.h"
#include "parameterlist.h"

LG_IMPLEMENT_DYNCREATE(PhongShader,Shader)

PhongShader::PhongShader() 
{
	diff=Color::GRAY;
	spec=Color::GRAY;
	power=20.0f;
	numRays=4;
}

BOOL PhongShader::update(ParameterList&pl,LGAPI&api) 
{
	diff=pl.getColor("diffuse",diff);
	spec=pl.getColor("specular",spec);
	power=pl.getFloat("power",power);
	numRays=pl.getInt("samples",numRays);

	return TRUE;
}

Color PhongShader::getDiffuse(ShadingState&state) const
{
	return diff;
}

Color PhongShader::getRadiance(ShadingState&state)const 
{
	state.faceforward();
	state.initLightSamples();
	state.initCausticSamples();

	return state.diffuse(getDiffuse(state)).add(state.specularPhong(spec,
		power,numRays));
}

void PhongShader::scatterPhoton(ShadingState&state,Color&_power)const
{	
	state.faceforward();
	Color d=getDiffuse(state);
	state.storePhoton(state.getRay().getDirection(),_power,d);
	float avgD=d.getAverage();
	float avgS=spec.getAverage();
	double rnd=state.getRandom(0,0,1);
	if(rnd<avgD) 
	{		
		_power.mul(d).mul(1.0f/avgD);
		OrthoNormalBasis onb=state.getBasis();
		double u=2.0*LG_PI* rnd/avgD;
		double v=state.getRandom(0,1,1);
		float s=(float)sqrt(v);
		float s1=(float)sqrt(1.0f-v);
		Vector3 w((float)cos(u)*s,(float)sin(u)*s,s1);
		w=onb.transform(w,Vector3());
		state.traceDiffusePhoton(Ray(state.getPoint(),w),_power);
	} 
	else if(rnd<avgD+avgS) 
	{		
		float dn=2.0f*state.getCosND();		
		Vector3 refDir;
		refDir.x=(dn*state.getNormal().x)+state.getRay().dx;
		refDir.y=(dn*state.getNormal().y)+state.getRay().dy;
		refDir.z=(dn*state.getNormal().z)+state.getRay().dz;
		_power.mul(spec).mul(1.0f/avgS);
		OrthoNormalBasis onb=state.getBasis();
		double u=2.0*LG_PI*(rnd-avgD)/avgS;
		double v=state.getRandom(0,1,1);
		float s=(float)pow(v,(double)1.0/(power+1));
		float s1=(float)sqrt(1.0-s* s);
		Vector3 w((float)cos(u)*s1,(float)sin(u)*s1,s);
		w=onb.transform(w,Vector3());
		state.traceReflectionPhoton(Ray(state.getPoint(),w),_power);
	}
}