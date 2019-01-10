#pragma once
#include "shader.h"

class ViewGlobalPhotonsShader :
	public Shader
{
	LG_DECLARE_DYNCREATE(ViewGlobalPhotonsShader);

public:
	BOOL update(ParameterList&pl,LGAPI&api);
	Color getRadiance(ShadingState&state)const;
    void scatterPhoton(ShadingState&state,Color&power)const;	
};
