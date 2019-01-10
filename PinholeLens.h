#pragma once

#include "CameraLens.h"

class PinholeLens : public CameraLens 
{
	LG_DECLARE_DYNCREATE(PinholeLens);

public:
	PinholeLens(void);

    BOOL update(ParameterList&pl,LGAPI&api);
	Ray getRay(float x,float y,int imageWidth,int imageHeight,
		double lensX,double lensY,double time)const;

private:
	float au,av;
	float aspect,fov;
	float shiftX,shiftY;

	void update();
};
