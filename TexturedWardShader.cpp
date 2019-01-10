#include "StdAfx.h"

#include "TexturedWardShader.h"
#include "ShadingState.h"
#include "texturecache.h"
#include "parameterlist.h"

LG_IMPLEMENT_DYNCREATE(TexturedWardShader,AnisotropicWardShader)

TexturedWardShader::TexturedWardShader()
{	
}

BOOL TexturedWardShader::update(ParameterList&pl,LGAPI&api) 
{
	CString fileName=pl.getString("texture",CString());
	if(!fileName.IsEmpty())
		tex=TextureCache::getTexture(fileName,FALSE);

	return RenderObject::update(pl,api);
}

Color TexturedWardShader::getDiffuse(ShadingState&state)const 
{
	return tex.getPixel(state.getUV().x,state.getUV().y);
}