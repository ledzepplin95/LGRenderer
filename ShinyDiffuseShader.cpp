#include "StdAfx.h"

#include "ShinyDiffuseShader.h"
#include "ShadingState.h"
#include "LGDef.h"
#include "parameterlist.h"

LG_IMPLEMENT_DYNCREATE(ShinyDiffuseShader,Shader)

ShinyDiffuseShader::ShinyDiffuseShader() 
{
	diff=Color::GRAY;
	refl=0.5f;
}

BOOL ShinyDiffuseShader::update(ParameterList&pl,LGAPI&api) 
{
	diff=pl.getColor("diffuse",diff);
	refl=pl.getFloat("shiny",refl);

	return TRUE;
}

Color ShinyDiffuseShader::getDiffuse(ShadingState&state)const
{
	return diff;
}

Color ShinyDiffuseShader::getRadiance(ShadingState&state) const
{	
	state.faceforward();
	state.initLightSamples();
	state.initCausticSamples();
	Color d=getDiffuse(state);
	Color lr=state.diffuse(d);
	if(!state.includeSpecular())
		return lr;
	float cos=state.getCosND();
	float dn=2.0f*cos;
	Vector3 refDir;
	refDir.x=(dn*state.getNormal().x)+state.getRay().getDirection().x;
	refDir.y=(dn*state.getNormal().y)+state.getRay().getDirection().y;
	refDir.z=(dn*state.getNormal().z)+state.getRay().getDirection().z;
	Ray refRay(state.getPoint(),refDir);	
	cos=1.0f-cos;
	float cos2=cos*cos;
	float cos5=cos2*cos2*cos;

	Color ret=Color::WHITE;
	Color r=d.copy().mul(refl);
	ret.sub(r);
	ret.mul(cos5);
	ret.add(r);

	return lr.add(ret.mul(state.traceReflection(refRay,0)));
}

void ShinyDiffuseShader::scatterPhoton(ShadingState&state,Color&power)const
{
	Color diffuse;	
	state.faceforward();
	diffuse=getDiffuse(state);
	state.storePhoton(state.getRay().getDirection(),power,diffuse);
	float d=diffuse.getAverage();
	float r=d*refl;
	double rnd=state.getRandom(0,0,1);
	if(rnd<d) 
	{		
		power.mul(diffuse).mul(1.0f/d);
		OrthoNormalBasis onb=state.getBasis();
		double u=2.0*LG_PI*rnd/d;
		double v=state.getRandom(0,1,1);
		float s=(float)sqrt(v);
		float s1=(float)sqrt(1.0-v);
		Vector3 w((float)cos(u)*s,(float)sin(u)*s,s1);
		w=onb.transform(w,Vector3());
		state.traceDiffusePhoton(Ray(state.getPoint(),w),power);
	} 
	else if(rnd<d+r)
	{
		float cos=-Vector3::dot(state.getNormal(),state.getRay().getDirection());
		power.mul(diffuse).mul(1.0f/d);	
		float dn=2.0f*cos;
		Vector3 dir;
		dir.x=(dn*state.getNormal().x)+state.getRay().getDirection().x;
		dir.y=(dn*state.getNormal().y)+state.getRay().getDirection().y;
		dir.z=(dn*state.getNormal().z)+state.getRay().getDirection().z;
		state.traceReflectionPhoton(Ray(state.getPoint(),dir),power);
	}
}
