#include "StdAfx.h"

#include "PinholeLens.h"
#include "MathUtil.h"
#include "ray.h"
#include "ParameterList.h"

LG_IMPLEMENT_DYNCREATE(PinholeLens,CameraLens)

PinholeLens::PinholeLens() 
{
	fov=90.0f;
	aspect=1.0f;
	shiftX=shiftY=0.0f;
	update();
}

BOOL PinholeLens::update(ParameterList&pl,LGAPI&api)
{	
	fov=pl.getFloat("fov",fov);
	aspect=pl.getFloat("aspect",aspect);
	shiftX=pl.getFloat("shift.x",shiftX);
	shiftY=pl.getFloat("shift.y",shiftY);
	update();

	return TRUE;
}

void PinholeLens::update() 
{
	au=(float)tan(DegToRad(fov*0.5f));
	av=au/aspect;
}

Ray PinholeLens::getRay(float x,float y,int imageWidth,int imageHeight,
		   double lensX,double lensY,double time) const
{
	float du=shiftX-au+((2.0f*au*x)/(imageWidth-1.0f));
	float dv=shiftY-av+((2.0f*av*y)/(imageHeight-1.0f));

	return Ray(0.0f,0.0f,0.0f,du,dv,-1.0f);
}