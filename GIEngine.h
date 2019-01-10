#pragma once

#include "lgobject.h"

class GIEngine : public LGObject
{
	LG_DECLARE_DYNCREATE(GIEngine);
public:
	virtual Color getGlobalRadiance(ShadingState&state)const;
	virtual BOOL init(Options*options,Scene*scene);
    virtual Color getIrradiance(ShadingState&state,const Color&diffuseReflectance)const;	
};
