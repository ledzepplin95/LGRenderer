#include "StdAfx.h"

#include "MirrorShader.h"
#include "ShadingState.h"
#include "parameterlist.h"

LG_IMPLEMENT_DYNCREATE(MirrorShader,Shader)

MirrorShader::MirrorShader() 
{
	color=Color::WHITE;
}

BOOL MirrorShader::update(ParameterList&pl,LGAPI&api)
{
	color=pl.getColor("color",color);

	return TRUE;
}

Color MirrorShader::getRadiance(ShadingState&state)const
{
	if(!state.includeSpecular())
		return Color::BLACK;
	state.faceforward();
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
	ret.sub(color);
	ret.mul(cos5);
	ret.add(color);

	return ret.mul(state.traceReflection(refRay,0));
}

void MirrorShader::scatterPhoton(ShadingState&state,Color&power)const
{
	float avg=color.getAverage();
	double rnd=state.getRandom(0,0,1);
	if(rnd>=avg)
		return;
	state.faceforward();
	float cos=state.getCosND();
	power.mul(color).mul(1.0f/avg);	
	float dn=2.0f*cos;
	Vector3 dir;
	dir.x=(dn*state.getNormal().x)+state.getRay().getDirection().x;
	dir.y=(dn*state.getNormal().y)+state.getRay().getDirection().y;
	dir.z=(dn*state.getNormal().z)+state.getRay().getDirection().z;
	state.traceReflectionPhoton(Ray(state.getPoint(),dir),power);
}