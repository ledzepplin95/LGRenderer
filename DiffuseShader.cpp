#include "StdAfx.h"

#include "DiffuseShader.h"
#include "LGDef.h"
#include "ShadingState.h"
#include "parameterlist.h"

LG_IMPLEMENT_DYNCREATE(DiffuseShader,Shader)

DiffuseShader::DiffuseShader() 
{
	diff=Color::WHITE;
}

BOOL DiffuseShader::update(ParameterList&pl,LGAPI&api)
{
	diff=pl.getColor("diffuse",diff);

	return TRUE;
}

Color DiffuseShader::getDiffuse(ShadingState&state)const
{
	return diff;
}

Color DiffuseShader::getRadiance(ShadingState&state) const
{	
	state.faceforward();	
	state.initLightSamples();
	state.initCausticSamples();

	return state.diffuse(getDiffuse(state));
}

void DiffuseShader::scatterPhoton(ShadingState&state,Color&power)const 
{
	Color diffuse;
	if( Vector3::dot(state.getNormal(),state.getRay().getDirection())>0.0f )
	{
		state.ss_normal().negate();
		state.ss_geoNormal().negate();
	}
	diffuse=getDiffuse(state);
	state.storePhoton(state.getRay().getDirection(),power,diffuse);
	float avg=diffuse.getAverage();
	double rnd=state.getRandom(0,0,1);
	if(rnd<avg) 
	{		
		power.mul(diffuse).mul(1.0f/avg);
		OrthoNormalBasis onb=state.getBasis();
		double u=2.0*LG_PI*rnd/avg;
		double v=state.getRandom(0,1,1);
		float s=(float)sqrt(v);
		float s1=(float)sqrt(1.0-v);
		Vector3 w((float)cos(u)*s,(float)sin(u)*s,s1);
		w=onb.transform(w,Vector3());
		state.traceDiffusePhoton(Ray(state.getPoint(),w),power);
	}
}