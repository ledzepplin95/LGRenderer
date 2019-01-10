#include "StdAfx.h"

#include "NormalMapModifier.h"
#include "ShadingState.h"
#include "TextureCache.h"
#include "lgapi.h"

LG_IMPLEMENT_DYNCREATE(NormalMapModifier,Modifier)

BOOL NormalMapModifier::update(ParameterList&pl,LGAPI&api)
{
	CString fileName=pl.getString("texture",CString());
	if(!fileName.IsEmpty())
		normalMap=TextureCache::getTexture(api.resolveTextureFilename(fileName),TRUE);

	return TRUE;
}

void NormalMapModifier::modify(ShadingState&state)const
{	
	state.ss_normal().set(normalMap.getNormal(state.getUV().x,state.getUV().y, 
		state.getBasis()));
	state.setBasis(OrthoNormalBasis::makeFromW(state.getNormal()));
}
