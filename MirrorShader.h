#pragma once
#include "shader.h"
#include "color.h"

class MirrorShader :
	public Shader
{
	LG_DECLARE_DYNCREATE(MirrorShader);

public:
    MirrorShader();

	BOOL update(ParameterList&pl,LGAPI&api);
	Color getRadiance(ShadingState&state)const;
	void scatterPhoton(ShadingState&state,Color&power)const;	

private:
    Color color;
};
