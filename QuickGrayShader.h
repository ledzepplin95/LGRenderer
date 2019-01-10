#pragma once
#include "shader.h"

class QuickGrayShader :
	public Shader
{
	LG_DECLARE_DYNCREATE(QuickGrayShader);

public:
    QuickGrayShader();

	BOOL update(ParameterList&pl,LGAPI&api);
	Color getRadiance(ShadingState&state)const;
    void scatterPhoton(ShadingState&state,Color&power)const;
};
