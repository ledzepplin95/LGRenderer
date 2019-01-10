#include "StdAfx.h"

#include "SimpleShader.h"
#include "ShadingState.h"

LG_IMPLEMENT_DYNCREATE(SimpleShader,Shader)

BOOL SimpleShader::update(ParameterList&pl,LGAPI&api) 
{
	return TRUE;
}

Color SimpleShader::getRadiance(ShadingState&state)const 
{
	return Color(fabs(state.getRay().dot(state.getNormal())));
}

void SimpleShader::scatterPhoton(ShadingState&state,Color&power)const
{
}