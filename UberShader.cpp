#include "StdAfx.h"

#include "UberShader.h"
#include "ShadingState.h"
#include "LGDef.h"
#include "TextureCache.h"
#include "MathUtil.h"
#include "parameterlist.h"

LG_IMPLEMENT_DYNCREATE(UberShader,Shader)

UberShader::UberShader() 
{
	diff=spec=Color::GRAY;	
	diffBlend=specBlend=1.0f;
	glossyness=0.0f;
	numSamples=4;
}

BOOL UberShader::update(ParameterList&pl,LGAPI&api)
{
	diff=pl.getColor("diffuse",diff);
	spec=pl.getColor("specular",spec);
	CString fileName;
	fileName=pl.getString("diffuse.texture",CString());
	if(!fileName.IsEmpty())
		diffmap=TextureCache::getTexture(fileName,FALSE);
	fileName=pl.getString("specular.texture",CString());
	if(!fileName.IsEmpty())
		specmap=TextureCache::getTexture(fileName,FALSE);
	diffBlend=MathUtil::clamp(pl.getFloat("diffuse.blend",diffBlend), 
		0.0f,1.0f);
	specBlend=MathUtil::clamp(pl.getFloat("specular.blend",diffBlend), 
		0.0f,1.0f);
	glossyness =MathUtil::clamp(pl.getFloat("glossyness",glossyness),
		0.0f,1.0f);
	numSamples=pl.getInt("samples",numSamples);

	return TRUE;
}

Color UberShader::getDiffuse(ShadingState&state) const
{
	return Color::blend(diff,diffmap.getPixel(state.getUV().x, 
		state.getUV().y),diffBlend);
}

Color UberShader::getSpecular(ShadingState&state) const
{
	return Color::blend(spec,specmap.getPixel(state.getUV().x, 
		state.getUV().y),specBlend);
}

Color UberShader::getRadiance(ShadingState&state)const 
{	
	state.faceforward();
	state.initLightSamples();
	state.initCausticSamples();
	Color d=getDiffuse(state);
	Color lr=state.diffuse(d);
	if(!state.includeSpecular())
		return lr;

	if(glossyness==0.0f) 
	{
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
		Color spec=getSpecular(state);
		Color ret=Color::WHITE;
		ret.sub(spec);
		ret.mul(cos5);
		ret.add(spec);

		return lr.add(ret.mul(state.traceReflection(refRay,0)));
	} 
	else
		return lr.add(state.specularPhong(getSpecular(state),
		2.0/glossyness,numSamples));
}

void UberShader::scatterPhoton(ShadingState&state,Color&power)const 
{
	Color diffuse,specular;	
	state.faceforward();
	diffuse=getDiffuse(state);
	specular=getSpecular(state);
	state.storePhoton(state.getRay().getDirection(),power,diffuse);
	float d=diffuse.getAverage();
	float r=specular.getAverage();
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
		if(glossyness==0.0f) 
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
		else 
		{
			float dn=2.0f*state.getCosND();		
			Vector3 refDir;
			refDir.x=(dn*state.getNormal().x)+state.getRay().dx;
			refDir.y=(dn*state.getNormal().y)+state.getRay().dy;
			refDir.z=(dn*state.getNormal().z)+state.getRay().dz;
			power.mul(spec).mul(1.0f/r);
			OrthoNormalBasis onb=state.getBasis();
			double u=2.0*LG_PI*(rnd-r)/r;
			double v=state.getRandom(0,1,1);
			float s=(float)pow(v,(double)1.0/((1.0f/glossyness)+1.0));
			float s1=(float)sqrt(1.0-s*s);
			Vector3 w((float)cos(u)*s1,(float)sin(u)*s1,s);
			w = onb.transform(w,Vector3());
			state.traceReflectionPhoton(Ray(state.getPoint(),w),power);
		}
	}
}