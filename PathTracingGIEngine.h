#pragma once
#include "giengine.h"

class PathTracingGIEngine :
	public GIEngine
{
	LG_DECLARE_DYNCREATE(PathTracingGIEngine);

public:
	Color getGlobalRadiance(ShadingState&state)const;
	BOOL init(Options*options,Scene*scene);
	Color getIrradiance(ShadingState&state,const Color&diffuseReflectance)const;

private:
	int samples;
};
