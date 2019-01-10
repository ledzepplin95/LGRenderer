#pragma once
#include "shader.h"

class UVShader :
	public Shader
{
    LG_DECLARE_DYNCREATE(UVShader);

public:
	BOOL update(ParameterList&pl,LGAPI&api);
    Color getRadiance(ShadingState&state)const;
	void scatterPhoton(ShadingState&state,Color&power)const;	
};
