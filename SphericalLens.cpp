#include "StdAfx.h"

#include "SphericalLens.h"
#include "lgdef.h"
#include "ray.h"

LG_IMPLEMENT_DYNCREATE(SphericalLens,CameraLens)

SphericalLens::SphericalLens(void)
{
}

BOOL SphericalLens::update(ParameterList&pl,LGAPI&api) 
{
	return TRUE;
}

Ray SphericalLens::getRay(float x,float y,int imageWidth,int imageHeight, 
				  double lensX,double lensY,double time)const 
{	
	double theta=2.0*LG_PI*x/imageWidth+LG_PI/2.0;
	double phi=LG_PI*(imageHeight-1.0-y)/imageHeight;

	return Ray(0,0,0,(float)(cos(theta)*sin(phi)),(float)(cos(phi)), 
		(float)(sin(theta)*sin(phi)));
}
