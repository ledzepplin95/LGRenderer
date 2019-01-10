#include "StdAfx.h"

#include "UVShader.h"
#include "ShadingState.h"

LG_IMPLEMENT_DYNCREATE(UVShader,Shader)

BOOL UVShader::update(ParameterList&pl,LGAPI&api) 
{
	return TRUE;
}

Color UVShader::getRadiance(ShadingState&state)const
{	
	return Color(state.getUV().x,state.getUV().y,0);
}

void UVShader::scatterPhoton(ShadingState&state,Color&power) const
{
}