#include "StdAfx.h"

#include "TexturedPhongShader.h"
#include "ShadingState.h"
#include "TextureCache.h"
#include "parameterlist.h"

LG_IMPLEMENT_DYNCREATE(TexturedPhongShader,PhongShader)

TexturedPhongShader::TexturedPhongShader()
{	
}

BOOL TexturedPhongShader::update(ParameterList&pl,LGAPI&api) 
{
	CString fileName=pl.getString("texture",CString());
	if(!fileName.IsEmpty())
		tex=TextureCache::getTexture(fileName,FALSE);

	return RenderObject::update(pl,api);
}

Color TexturedPhongShader::getDiffuse(ShadingState&state)const 
{
	return tex.getPixel(state.getUV().x,state.getUV().y);
}