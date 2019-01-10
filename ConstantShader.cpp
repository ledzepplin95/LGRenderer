#include "StdAfx.h"
#include "ConstantShader.h"
#include "parameterlist.h"

LG_IMPLEMENT_DYNCREATE(ConstantShader,Shader)

ConstantShader::ConstantShader()
{
	c=Color::WHITE;
}

BOOL ConstantShader::update(ParameterList&pl,LGAPI&api) 
{
	c=pl.getColor("color",c);

	return TRUE;
}

Color ConstantShader::getRadiance(ShadingState&state)const
{
	return c;
}

void ConstantShader::scatterPhoton(ShadingState&state,Color&power)const 
{
}