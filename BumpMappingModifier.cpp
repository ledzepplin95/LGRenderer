#include "StdAfx.h"

#include "BumpMappingModifier.h"
#include "ShadingState.h"
#include "TextureCache.h"
#include "lgapi.h"

LG_IMPLEMENT_DYNCREATE(BumpMappingModifier,Modifier)

BumpMappingModifier::BumpMappingModifier()
{	
	scale=1.0f;
}

BOOL BumpMappingModifier::update(ParameterList&pl,LGAPI&api)
{
	CString fileName=pl.getString("texture",CString());
	if(!fileName.IsEmpty())
		bumpTexture=TextureCache::getTexture(api.resolveTextureFilename(fileName),TRUE);
	scale=pl.getFloat("scale",scale);

	return TRUE;
}

void BumpMappingModifier::modify(ShadingState&state) const
{
	state.ss_normal().set(bumpTexture.getBump(state.getUV().x,
		state.getUV().y,state.getBasis(),scale));
	state.setBasis(OrthoNormalBasis::makeFromW(state.getNormal()));
}