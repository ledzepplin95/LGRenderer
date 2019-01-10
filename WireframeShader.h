#pragma once
#include "shader.h"
#include "color.h"

class WireframeShader :
	public Shader
{
	LG_DECLARE_DYNCREATE(WireframeShader);

public:
	WireframeShader();

	BOOL update(ParameterList&pl,LGAPI&api);
	Color getFillColor(ShadingState&state)const;
	Color getLineColor(ShadingState&state)const;
	Color getRadiance(ShadingState&state)const;
	void scatterPhoton(ShadingState&state,Color&power)const;

private:
	Color lineColor;
	Color fillColor;
	float width;
	float cosWidth;	
};
