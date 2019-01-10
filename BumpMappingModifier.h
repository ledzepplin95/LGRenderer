#pragma once
#include "modifier.h"
#include "Texture.h"

class BumpMappingModifier :
	public Modifier
{
	LG_DECLARE_DYNCREATE(BumpMappingModifier);

public:
	BumpMappingModifier();

	BOOL update(ParameterList&pl,LGAPI&api);
	void modify(ShadingState&state)const;

private:
	Texture bumpTexture;
    float scale;	
};
