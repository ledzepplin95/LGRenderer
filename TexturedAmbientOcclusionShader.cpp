#include "StdAfx.h"

#include "TexturedAmbientOcclusionShader.h"
#include "ShadingState.h"
#include "TextureCache.h"
#include "parameterlist.h"

LG_IMPLEMENT_DYNCREATE(TexturedAmbientOcclusionShader,AmbientOcclusionShader)

TexturedAmbientOcclusionShader::TexturedAmbientOcclusionShader() 
{	
}

BOOL TexturedAmbientOcclusionShader::update(ParameterList&pl,LGAPI&api)
{
	CString fileName=pl.getString("texture",CString());
	if(!fileName.IsEmpty())
		tex=TextureCache::getTexture(fileName,FALSE);

	return RenderObject::update(pl,api);
}

Color TexturedAmbientOcclusionShader::getBrightColor(ShadingState&state)const
{
	return tex.getPixel(state.getUV().x,state.getUV().y);
}