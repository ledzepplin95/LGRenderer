#pragma once
#include "shader.h"
#include "color.h"

class AmbientOcclusionShader :
	public Shader
{
	LG_DECLARE_DYNCREATE(AmbientOcclusionShader);

public:
	AmbientOcclusionShader();
	AmbientOcclusionShader(const Color&c,float d);

	BOOL update(ParameterList&pl,LGAPI&api);
	Color getBrightColor(ShadingState&state)const;
	Color getRadiance(ShadingState&state)const;
	void scatterPhoton(ShadingState&state,Color&power)const;

private:
	Color bright;
	Color dark;
	int samples;
	float maxDist;
};
