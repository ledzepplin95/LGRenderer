#pragma once
#include "shader.h"

class PrimIDShader :
	public Shader
{
	LG_DECLARE_DYNCREATE(PrimIDShader);

public:
	BOOL update(ParameterList&pl,LGAPI&api);
	Color getRadiance(ShadingState&state)const;
	void scatterPhoton(ShadingState&state,Color&power)const;

private:
	static Color BORDERS[];
};
