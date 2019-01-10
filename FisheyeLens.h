#pragma once

#include "CameraLens.h"

class FisheyeLens : public CameraLens
{
	LG_DECLARE_DYNCREATE(FisheyeLens);

public:
	FisheyeLens(void);

	Ray getRay(float x,float y,int imageWidth,int imageHeight,
		double lensX,double lensY,double time)const;
    BOOL update(ParameterList&pl,LGAPI&api);	
};
