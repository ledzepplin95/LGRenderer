#include "StdAfx.h"

#include "IDShader.h"
#include "ShadingState.h"

LG_IMPLEMENT_DYNCREATE(IDShader,Shader)

BOOL IDShader::update(ParameterList&pl,LGAPI&api)
{
	return TRUE;
}

Color IDShader::getRadiance(ShadingState&state) const
{
	Vector3 n=state.getNormal();
	float f=fabs(state.getRay().dot(n));

	return Color(Color().mul(f));
}

void IDShader::scatterPhoton(ShadingState&state,Color&power)const 
{
}