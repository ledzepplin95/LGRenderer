#include "StdAfx.h"

#include "NormalShader.h"
#include "ShadingState.h"

LG_IMPLEMENT_DYNCREATE(NormalShader,Shader)

BOOL NormalShader::update(ParameterList&pl,LGAPI&api) 
{
	return TRUE;
}

Color NormalShader::getRadiance(ShadingState&state)const
{
	Vector3 n=state.getNormal();	
	float r=(n.x+1.0)*0.5f;
	float g=(n.y+1.0)*0.5f;
	float b=(n.z+1.0)*0.5f;

	return Color(r,g,b);
}

void NormalShader::scatterPhoton(ShadingState&state,Color&power)const
{
}