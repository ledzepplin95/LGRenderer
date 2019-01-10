#pragma once
#include "shader.h"
#include "color.h"

class PhongShader :
	public Shader
{
	LG_DECLARE_DYNCREATE(PhongShader);

public:
	PhongShader();

	BOOL update(ParameterList&pl,LGAPI&api);
	Color getDiffuse(ShadingState&state)const;
	Color getRadiance(ShadingState&state)const;
	void scatterPhoton(ShadingState&state,Color&_power)const;

private:
	Color diff;
	Color spec;
	float power;
	int numRays;	
};
