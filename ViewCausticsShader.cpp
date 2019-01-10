#include "StdAfx.h"

#include "ViewCausticsShader.h"
#include "ShadingState.h"
#include "LGDef.h"

LG_IMPLEMENT_DYNCREATE(ViewCausticsShader,Shader)

BOOL ViewCausticsShader::update(ParameterList&pl,LGAPI&api)
{
	return TRUE;
}

Color ViewCausticsShader::getRadiance(ShadingState&state)const 
{
	state.faceforward();
	state.initCausticSamples();
	Color lr=Color::BLACK;
	for(LightSample*sample=state.ss_lightSampleHead(); sample!=NULL; sample=sample->next)
		lr.madd(sample->dot(state.getNormal()),sample->getDiffuseRadiance());

	return lr.mul(1.0f/(float)LG_PI);
}

void ViewCausticsShader::scatterPhoton(ShadingState&state,Color&power)const 
{
}