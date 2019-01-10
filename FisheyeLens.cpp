#include "StdAfx.h"

#include "FisheyeLens.h"
#include "ray.h"

LG_IMPLEMENT_DYNCREATE(FisheyeLens,CameraLens)

FisheyeLens::FisheyeLens(void)
{
}

BOOL FisheyeLens::update(ParameterList&pl,LGAPI&api) 
{
	return TRUE;
}

Ray FisheyeLens::getRay(float x,float y,int imageWidth,int imageHeight,
						double lensX,double lensY,double time) const
{
	float cx=2.0f*x/imageWidth-1.0f;
	float cy=2.0f*y/imageHeight-1.0f;
	float r2=cx*cx+cy*cy;
	if(r2>1.0f)
		return Ray(); 

	return Ray(0.0f,0.0f,0.0f,cx,cy,(float)-sqrt(1.0f-r2));
}