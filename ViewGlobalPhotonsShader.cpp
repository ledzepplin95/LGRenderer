#include "StdAfx.h"

#include "ViewGlobalPhotonsShader.h"
#include "ShadingState.h"

LG_IMPLEMENT_DYNCREATE(ViewGlobalPhotonsShader,Shader)

BOOL ViewGlobalPhotonsShader::update(ParameterList&pl,LGAPI&api)
{
	return TRUE;
}

Color ViewGlobalPhotonsShader::getRadiance(ShadingState&state) const
{
	state.faceforward();

	return state.getGlobalRadiance();
}

void ViewGlobalPhotonsShader::scatterPhoton(ShadingState&state,Color&power)const 
{
}