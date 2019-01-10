#pragma once
#include "diffuseshader.h"
#include "Texture.h"

class TexturedDiffuseShader :
	public DiffuseShader
{
	LG_DECLARE_DYNCREATE(TexturedDiffuseShader);

public:
	TexturedDiffuseShader();

	BOOL update(ParameterList&pl,LGAPI&api);
	Color getDiffuse(ShadingState&state)const;

private:
    Texture tex;	
};
