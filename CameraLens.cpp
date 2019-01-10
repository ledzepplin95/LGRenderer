#include "stdafx.h"
#include "cameralens.h"
#include "ray.h"

LG_IMPLEMENT_DYNCREATE(CameraLens,RenderObject)

Ray CameraLens::getRay(float x,float y,int imageWidth,int imageHeight,
				   double lensX,double lensY,double time)const
{
	return Ray();
}