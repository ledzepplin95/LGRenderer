#pragma once
#include "shader.h"

class ViewIrradianceShader :
	public Shader
{
	LG_DECLARE_DYNCREATE(ViewIrradianceShader);

public: 
    BOOL update(ParameterList&pl,LGAPI&api);
	Color getRadiance(ShadingState&state)const;
	void scatterPhoton(ShadingState&state,Color&power)const;
};
