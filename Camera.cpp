#include "StdAfx.h"

#include "Camera.h"
#include "ray.h"
#include "parameterlist.h"
#include "point3.h"
#include "CameraLens.h"
#include "StrUtil.h"

LG_IMPLEMENT_DYNCREATE(Camera,RenderObject)

Camera::Camera()
{
}

Camera::Camera(CameraLens*ls)
{
	lens=ls;	
	shutterOpen=shutterClose=0.0f;
}

BOOL Camera::update(ParameterList&pl,LGAPI&api)
{
	shutterOpen=pl.getFloat("shutter.open",shutterOpen);
	shutterClose=pl.getFloat("shutter.close",shutterClose);
	c2w=pl.getMovingMatrix("transform",c2w);
	w2c=c2w.inverse();
	if(w2c.isNull())
	{
		StrUtil::PrintPrompt("不能计算相机的逆矩阵");
		return FALSE;
	}

	return lens->update(pl,api);
}

float Camera::getTime(float time)const
{
	if(shutterOpen>=shutterClose)
		return shutterOpen;
	if(time<0.5f)
		time=-1.0f+(float)sqrt(2.0*time);
	else
		time=1.0f-(float)sqrt(2.0-2.0*time);
	time=0.5f*(time+1);

	return (1.0f-time)*shutterOpen+time*shutterClose;
}

Ray Camera::getRay(float x,float y,int imageWidth,int imageHeight, 
				  double lensX,double lensY,float time)const
{
	Ray r=lens->getRay(x,y,imageWidth,imageHeight,lensX,lensY,time);
	if(!r.isNull())
	{
		r=r.transform(c2w.sample(time));		
		r.normalize();
	}

	return r;
}

Ray Camera::getRay(const Point3&p,float time)const
{
	return Ray(c2w.isNull()?Point3(0,0,0):c2w.sample(time).transformP(Point3(0,0,0)),p);
}

Matrix4 Camera::getCameraToWorld(float time) const
{
	return c2w.isNull()?Matrix4::IDENTITY:c2w.sample(time);
}

Matrix4 Camera::getWorldToCamera(float time) const
{
	return w2c.isNull()?Matrix4::IDENTITY:w2c.sample(time);
}

Camera& Camera::operator =(const Camera &c)
{
	if(this==&c) return *this;

	lens=c.lens;
	shutterOpen=c.shutterOpen;
	shutterClose=c.shutterClose;
	c2w=c.c2w;
	w2c=c.w2c;

	return *this;
}