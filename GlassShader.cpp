#include "StdAfx.h"

#include "GlassShader.h"
#include "ShadingState.h"
#include "parameterlist.h"

LG_IMPLEMENT_DYNCREATE(GlassShader,Shader)

GlassShader::GlassShader() 
{
	eta=1.3f;
	color=Color::WHITE;
	absorptionDistance=0.0f; 
	absorptionColor=Color::GRAY;
}

BOOL GlassShader::update(ParameterList&pl,LGAPI&api) 
{
	color=pl.getColor("color",color);
	eta=pl.getFloat("eta",eta);
	f0=(1.0f-eta)/(1.0f+eta);
	f0=f0*f0;
	absorptionDistance=pl.getFloat("absorption.distance",absorptionDistance);
	absorptionColor=pl.getColor("absorption.color",absorptionColor);

	return TRUE;
}

Color GlassShader::getRadiance(ShadingState&state)const
{
	if(!state.includeSpecular())
		return Color::BLACK;
	Vector3 reflDir;
	Vector3 refrDir;
	state.faceforward();
	float cos=state.getCosND();
	BOOL inside=state.isBehind();
	float neta=inside?eta:1.0f/eta;

	float dn=2.0f*cos;
	reflDir.x=(dn*state.getNormal().x)+state.getRay().getDirection().x;
	reflDir.y=(dn*state.getNormal().y)+state.getRay().getDirection().y;
	reflDir.z=(dn*state.getNormal().z)+state.getRay().getDirection().z;
	
	float arg=1.0f-(neta*neta*(1.0f-(cos*cos)));
	BOOL tir=arg<0.0f;
	if(tir)
		refrDir.x=refrDir.y=refrDir.z=0.0f;
	else 
	{
		float nK=(neta*cos)-(float)sqrt(arg);
		refrDir.x=(neta*state.getRay().dx)+(nK*state.getNormal().x);
		refrDir.y=(neta*state.getRay().dy)+(nK*state.getNormal().y);
		refrDir.z=(neta*state.getRay().dz)+(nK*state.getNormal().z);
	}

	float cosTheta1=Vector3::dot(state.getNormal(),reflDir);
	float cosTheta2=-Vector3::dot(state.getNormal(),refrDir);

	float pPara=(cosTheta1-eta*cosTheta2)/(cosTheta1+eta*cosTheta2);
	float pPerp=(eta*cosTheta1-cosTheta2)/(eta*cosTheta1+cosTheta2);
	float kr=0.5f*(pPara*pPara+pPerp*pPerp);
	float kt=1.0f-kr;

	Color absorbtion;
	if( inside && absorptionDistance>0.0f ) 
	{		
		absorbtion=Color::mul(
			-state.getRay().getMax()/absorptionDistance,
			absorptionColor.copy().opposite() ).exp();
		if(absorbtion.isBlack())
			return Color::BLACK;
	}	
	Color ret=Color::BLACK;
	if(!tir)
	{
		ret.madd(kt,state.traceRefraction(Ray(state.getPoint(),
			refrDir),0)).mul(color);
	}
	if(!inside || tir)
		ret.add(Color::mul(kr, state.traceReflection(Ray(state.getPoint(),
		reflDir),0)).mul(color));

	return absorbtion!=Color()?ret.mul(absorbtion):ret;
}

void GlassShader::scatterPhoton(ShadingState&state,Color&power)const 
{
	Color refr=Color::mul(1-f0,color);
	Color refl=Color::mul(f0,color);
	float avgR=refl.getAverage();
	float avgT=refr.getAverage();
	double rnd=state.getRandom(0,0,1);
	if(rnd<avgR) 
	{
		state.faceforward();		
		if(state.isBehind())
			return;		
		float cos=state.getCosND();
		power.mul(refl).mul(1.0f/avgR);
		float dn=2.0f*cos;
		Vector3 dir;
		dir.x=(dn*state.getNormal().x)+state.getRay().getDirection().x;
		dir.y=(dn*state.getNormal().y)+state.getRay().getDirection().y;
		dir.z=(dn*state.getNormal().z)+state.getRay().getDirection().z;
		state.traceReflectionPhoton(Ray(state.getPoint(),dir),power);
	} 
	else if(rnd<avgR+avgT) 
	{
		state.faceforward();		
		float cos=state.getCosND();
		float neta=state.isBehind()?eta:1.0f/eta;
		power.mul(refr).mul(1.0f/avgT);
		float wK=-neta;
		float arg=1.0f-(neta*neta*(1.0f-(cos*cos)));
		Vector3 dir;
		if(state.isBehind() && absorptionDistance>0.0f) 
		{			
			power.mul(Color::mul(-state.getRay().getMax()/absorptionDistance, 
				absorptionColor.copy().opposite()).exp());
		}
		if(arg<0.0f)
		{			
			float dn=2.0f*cos;
			dir.x=(dn*state.getNormal().x)+state.getRay().getDirection().x;
			dir.y=(dn*state.getNormal().y)+state.getRay().getDirection().y;
			dir.z=(dn*state.getNormal().z)+state.getRay().getDirection().z;
			state.traceReflectionPhoton(Ray(state.getPoint(),dir),power);
		}
		else 
		{
			float nK=neta*cos-(float)sqrt(arg);
			dir.x=(-wK*state.getRay().dx)+(nK*state.getNormal().x);
			dir.y=(-wK*state.getRay().dy)+(nK*state.getNormal().y);
			dir.z=(-wK*state.getRay().dz)+(nK*state.getNormal().z);
			state.traceRefractionPhoton(Ray(state.getPoint(),dir),power);
		}
	}
}