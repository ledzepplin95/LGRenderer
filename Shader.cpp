#include "stdafx.h"

#include "shader.h"
#include "color.h"

LG_IMPLEMENT_DYNCREATE(Shader,RenderObject)

Color Shader::getRadiance(ShadingState&state)const
{
	return Color();
}

void Shader::scatterPhoton(ShadingState&state,Color&power)const
{
}

BOOL Shader::operator ==(const Shader&s) const
{
	return FALSE;
}

BOOL Shader::operator !=(const Shader&s)const
{
	return FALSE;
}