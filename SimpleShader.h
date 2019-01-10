#pragma once
#include "shader.h"

class SimpleShader :
	public Shader
{
	LG_DECLARE_DYNCREATE(SimpleShader);
public:
	BOOL update(ParameterList&pl,LGAPI&api);
	Color getRadiance(ShadingState&state)const;
	void scatterPhoton(ShadingState&state,Color&power)const;	
};
