#pragma once
#include "anisotropicwardshader.h"
#include "Texture.h"

class TexturedWardShader :
	public AnisotropicWardShader
{
	LG_DECLARE_DYNCREATE(TexturedWardShader);

public:
	TexturedWardShader();

	 BOOL update(ParameterList&pl,LGAPI&api);
	 Color getDiffuse(ShadingState&state)const;

private:
	Texture tex;
};
