#pragma once
#include "shader.h"

class ViewCausticsShader :
	public Shader
{
	LG_DECLARE_DYNCREATE(ViewCausticsShader);

public:
	BOOL update(ParameterList&pl,LGAPI&api);
	Color getRadiance(ShadingState&state)const;
	void scatterPhoton(ShadingState&state,Color&power)const;
};
