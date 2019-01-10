#pragma once
#include "AmbientOcclusionShader.h"
#include "Texture.h"

class TexturedAmbientOcclusionShader :
	public AmbientOcclusionShader
{
	LG_DECLARE_DYNCREATE(TexturedAmbientOcclusionShader);

public:
	TexturedAmbientOcclusionShader();

	BOOL update(ParameterList&pl,LGAPI&api);
	Color getBrightColor(ShadingState&state)const;

private:
	Texture tex;	
};
