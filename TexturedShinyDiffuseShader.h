#pragma once
#include "shinydiffuseshader.h"
#include "Texture.h"

class TexturedShinyDiffuseShader :
	public ShinyDiffuseShader
{
	LG_DECLARE_DYNCREATE(TexturedShinyDiffuseShader);

public:
	TexturedShinyDiffuseShader();

	BOOL update(ParameterList&pl,LGAPI&api);
	Color getDiffuse(ShadingState&state)const;

private:
	Texture tex;	
};
