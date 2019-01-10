#pragma once
#include "phongshader.h"
#include "Texture.h"

class TexturedPhongShader :
	public PhongShader
{
	LG_DECLARE_DYNCREATE(TexturedPhongShader);

public:
	TexturedPhongShader();

	BOOL update(ParameterList&pl,LGAPI&api);
	Color getDiffuse(ShadingState&state)const;

private:
	Texture tex;
};
