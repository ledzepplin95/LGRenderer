#pragma once

#include "lgobject.h"

class ImageSampler : public LGObject
{
    LG_DECLARE_DYNCREATE(ImageSampler);
public:
	virtual BOOL prepare(Options*options,Scene*scene,int w,int h);
	virtual void render(Display*display);
};
