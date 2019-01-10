#pragma once
#include "giengine.h"
#include "color.h"

class AmbientOcclusionGIEngine :
	public GIEngine
{
	LG_DECLARE_DYNCREATE(AmbientOcclusionGIEngine);

public:
	Color getGlobalRadiance(ShadingState&state)const;
	BOOL init(Options*options,Scene*scene);
	Color getIrradiance(ShadingState &state,const Color&diffuseReflectance)const;

private:
	Color bright;
	Color dark;
	int samples;
	float maxDist;	
};
