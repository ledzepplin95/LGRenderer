#pragma once
#include "shader.h"
#include "Texture.h"
#include "color.h"

class UberShader :
	public Shader
{
	LG_DECLARE_DYNCREATE(UberShader);

public:
	UberShader();

	BOOL update(ParameterList&pl,LGAPI&api);
    Color getDiffuse(ShadingState&state)const;
	Color getSpecular(ShadingState&state)const;
	Color getRadiance(ShadingState&state)const;
	void scatterPhoton(ShadingState&state,Color&power)const;

private:
	Color diff;
	Color spec;
	Texture diffmap;
	Texture specmap;
	float diffBlend;
	float specBlend;
	float glossyness;
	int numSamples;	
};
