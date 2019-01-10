#pragma once
#include "shader.h"

class NormalShader :
	public Shader
{
	LG_DECLARE_DYNCREATE(NormalShader);

public:
    BOOL update(ParameterList&pl,LGAPI&api);
	Color getRadiance(ShadingState&state)const;
	void scatterPhoton(ShadingState&state,Color&power)const;	
};
