#include "stdafx.h"

#include "giengine.h"
#include "color.h"

LG_IMPLEMENT_DYNCREATE(GIEngine,LGObject)

Color GIEngine::getGlobalRadiance(ShadingState&state)const
{
	return Color();
}

BOOL GIEngine::init(Options*options,Scene*scene)
{
	return FALSE;
}

Color GIEngine::getIrradiance(ShadingState&state,const Color&diffuseReflectance)const
{
	return Color();
}