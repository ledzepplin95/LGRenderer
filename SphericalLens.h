#pragma once

#include "CameraLens.h"

class SphericalLens : public CameraLens
{
	LG_DECLARE_DYNCREATE(SphericalLens);

public:
	SphericalLens(void);

    BOOL update(ParameterList&pl,LGAPI&api);
	Ray getRay(float x,float y,int imageWidth,int imageHeight, 
		double lensX,double lensY,double time)const; 	
};
