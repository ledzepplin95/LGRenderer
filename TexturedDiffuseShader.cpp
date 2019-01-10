#include "StdAfx.h"

#include "TexturedDiffuseShader.h"
#include "ShadingState.h"
#include "TextureCache.h"
#include "parameterlist.h"

LG_IMPLEMENT_DYNCREATE(TexturedDiffuseShader,DiffuseShader)

TexturedDiffuseShader::TexturedDiffuseShader() 
{
}

BOOL TexturedDiffuseShader::update(ParameterList&pl,LGAPI&api)
{
	CString fileName=pl.getString("texture",CString());
	if(!fileName.IsEmpty())
		tex=TextureCache::getTexture(fileName,FALSE);

	return RenderObject::update(pl,api);
}

Color TexturedDiffuseShader::getDiffuse(ShadingState&state) const
{
	return tex.getPixel(state.getUV().x,state.getUV().y);
}