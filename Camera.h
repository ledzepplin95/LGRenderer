#pragma once

#include "MovingMatrix4.h"
#include "renderobject.h"

class Camera : public RenderObject
{
	LG_DECLARE_DYNCREATE(Camera);

public:
	Camera();
	Camera(CameraLens*ls);

	BOOL update(ParameterList&pl,LGAPI&api);
	Camera& operator=(const Camera&c);

	float getTime(float time)const;
	Ray getRay(float x,float y,int imageWidth,int imageHeight, 
		double lensX,double lensY,float time)const;
	Ray getRay(const Point3&p,float time)const;
	Matrix4 getCameraToWorld(float time)const;
	Matrix4 getWorldToCamera(float time)const;   

private:
	CameraLens* lens;
	float shutterOpen;
	float shutterClose;
	MovingMatrix4 c2w;
	MovingMatrix4 w2c;	
};
