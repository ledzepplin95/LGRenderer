#pragma once
#include "shader.h"
#include "color.h"

class DiffuseShader :
	public Shader
{
	LG_DECLARE_DYNCREATE(DiffuseShader);

public:
	DiffuseShader();

	BOOL update(ParameterList&pl,LGAPI&api);
	Color getDiffuse(ShadingState&state)const;
	Color getRadiance(ShadingState&state)const;
	void scatterPhoton(ShadingState&state,Color&power)const;

private:
	Color diff;	
};
