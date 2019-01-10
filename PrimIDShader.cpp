#include "StdAfx.h"

#include "PrimIDShader.h"
#include "ShadingState.h"

Color PrimIDShader::BORDERS[]={Color::RED,Color::GREEN,
Color::BLUE,Color::YELLOW,Color::CYAN,Color::MAGENTA};

LG_IMPLEMENT_DYNCREATE(PrimIDShader,Shader)

BOOL PrimIDShader::update(ParameterList&pl,LGAPI&api) 
{
	return TRUE;
}

Color PrimIDShader::getRadiance(ShadingState&state) const
{
	Vector3 n=state.getNormal();
	float f=fabs(state.getRay().dot(n));

	return BORDERS[state.getPrimitiveID()%6].copy().mul(f);	
}

void PrimIDShader::scatterPhoton(ShadingState&state,Color&power)const 
{
}