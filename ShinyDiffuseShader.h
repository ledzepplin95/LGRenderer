#pragma once
#include "shader.h"
#include "color.h"

class ShinyDiffuseShader :
	public Shader
{
	LG_DECLARE_DYNCREATE(ShinyDiffuseShader);

public:
	ShinyDiffuseShader();

	BOOL update(ParameterList&pl,LGAPI&api);
	Color getDiffuse(ShadingState&state)const;
	Color getRadiance(ShadingState&state)const;
	void scatterPhoton(ShadingState&state,Color&power)const;

private:
	Color diff;
	float refl;	
};
