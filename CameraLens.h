#pragma once

#include "RenderObject.h"

class CameraLens : public RenderObject
{
	LG_DECLARE_DYNCREATE(CameraLens);
public:	
    virtual Ray getRay(float x,float y,int imageWidth,int imageHeight,
		double lensX,double lensY,double time)const;	
};
