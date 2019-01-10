#pragma once
#include "shader.h"
#include "color.h"

class ConstantShader :
	public Shader
{
	LG_DECLARE_DYNCREATE(ConstantShader);

public:
	ConstantShader();

	BOOL update(ParameterList&pl,LGAPI&api);
	Color getRadiance(ShadingState&state)const;
    void scatterPhoton(ShadingState&state,Color&power)const;

private:
	Color c;
};
