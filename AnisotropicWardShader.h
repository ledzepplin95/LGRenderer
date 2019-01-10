#pragma once
#include "shader.h"
#include "color.h"

class Vector3;
class OrthoNormalBasis;
class AnisotropicWardShader :
	public Shader
{
	LG_DECLARE_DYNCREATE(AnisotropicWardShader);

public:
	AnisotropicWardShader();

    BOOL update(ParameterList&pl,LGAPI&api);
	Color getDiffuse(ShadingState&state)const;
    Color getRadiance(ShadingState&state)const;
	void scatterPhoton(ShadingState&state,Color&power)const;

private:
	Color rhoD;
	Color rhoS;
	float alphaX;
	float alphaY;
	int numRays;

	float brdf(const Vector3&i,const Vector3&o,const OrthoNormalBasis&basis)const;	
};
