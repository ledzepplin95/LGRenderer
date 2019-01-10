#include "StdAfx.h"

#include "ViewIrradianceShader.h"
#include "ShadingState.h"
#include "LGDef.h"

LG_IMPLEMENT_DYNCREATE(ViewIrradianceShader,Shader)

BOOL ViewIrradianceShader::update(ParameterList&pl,LGAPI&api) 
{
	return TRUE;
}

Color ViewIrradianceShader::getRadiance(ShadingState&state)const 
{
	state.faceforward();

	return Color().set(state.getIrradiance(Color::WHITE)).mul(1.0f/(float)LG_PI);
}

void ViewIrradianceShader::scatterPhoton(ShadingState&state,Color&power)const 
{
}