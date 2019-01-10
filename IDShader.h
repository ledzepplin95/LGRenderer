#pragma once
#include "shader.h"

class IDShader :
	public Shader
{
	LG_DECLARE_DYNCREATE(IDShader);

public:
	BOOL update(ParameterList&pl,LGAPI&api);
	Color getRadiance(ShadingState&state)const;
	void scatterPhoton(ShadingState&state,Color&power)const;
};
