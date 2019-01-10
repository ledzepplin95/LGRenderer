#pragma once
#include "modifier.h"
#include "Texture.h"

class NormalMapModifier :
	public Modifier
{
	LG_DECLARE_DYNCREATE(NormalMapModifier);

public:
	BOOL update(ParameterList&pl,LGAPI&api);
	void modify(ShadingState&state)const;

private:
	Texture normalMap;	
};
