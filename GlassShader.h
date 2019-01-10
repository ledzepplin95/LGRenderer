#pragma once
#include "shader.h"
#include "color.h"
#include "texture.h"

class GlassShader :
	public Shader
{
	LG_DECLARE_DYNCREATE(GlassShader);

public:
    GlassShader();

    BOOL update(ParameterList&pl,LGAPI&api);
	Color getRadiance(ShadingState&state)const;
	void scatterPhoton(ShadingState&state,Color&power)const;

private:
	float eta;
	float f0;
	Color color;
	float absorptionDistance;
	Color absorptionColor;	
};
