#include "StdAfx.h"

#include "AmbientOcclusionShader.h"
#include "float.h"
#include "ShadingState.h"
#include "parameterlist.h"

LG_IMPLEMENT_DYNCREATE(AmbientOcclusionShader,Shader)

AmbientOcclusionShader::AmbientOcclusionShader() 
{
	bright=Color::WHITE;
	dark=Color::BLACK;
	samples=32;
	maxDist=FLT_MAX;
}

AmbientOcclusionShader::AmbientOcclusionShader(const Color&c,float d) 
{
	bright=Color::WHITE;
	dark=Color::BLACK;
	samples=32;
	maxDist=FLT_MAX;
	bright=c;
	maxDist=d;
}

BOOL AmbientOcclusionShader::update(ParameterList&pl,LGAPI&api) 
{
	bright=pl.getColor("bright",bright);
	dark=pl.getColor("dark",dark);
	samples=pl.getInt("samples",samples);
	maxDist=pl.getFloat("maxdist",maxDist);
	if(maxDist<=0.0f)
		maxDist=FLT_MAX;

	return TRUE;
}

Color AmbientOcclusionShader::getBrightColor(ShadingState&state) const
{
	return bright;
}

Color AmbientOcclusionShader::getRadiance(ShadingState&state)const 
{
	return state.occlusion(samples,maxDist,getBrightColor(state),dark);
}

void AmbientOcclusionShader::scatterPhoton(ShadingState&state,Color&power)const 
{
}