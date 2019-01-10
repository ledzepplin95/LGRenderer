#pragma once
#include "giengine.h"
#include "vector3.h"
#include "color.h"

class FakeGIEngine :
	public GIEngine
{
	LG_DECLARE_DYNCREATE(FakeGIEngine);

public:
	Color getGlobalRadiance(ShadingState&state)const;
	BOOL init(Options*options,Scene*scene);
	Color getIrradiance(ShadingState&tate,const Color&diffuseReflectance)const;

private:
	Vector3 up;
	Color sky;
	Color ground;	
};
