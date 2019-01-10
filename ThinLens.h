#pragma once

#include "CameraLens.h"

class ThinLens : public CameraLens
{
	LG_DECLARE_DYNCREATE(ThinLens);

public:
	ThinLens(void);
	virtual ~ThinLens(void){};

    BOOL update(ParameterList&pl,LGAPI&api);
	Ray getRay(float x,float y,int imageWidth,int imageHeight,
		double lensX,double lensY,double time)const;

private:
	float au,av;
	float aspect,fov;
	float shiftX,shiftY;
	float focusDistance;
	float lensRadius;
	int lensSides;
	float lensRotation;
	float lensRotationRadians;

	void update();
};
