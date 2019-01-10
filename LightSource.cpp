#include "stdafx.h"

#include "LightSource.h"
#include "point3.h"
#include "color.h"
#include "Instance.h"
#include "Vector3.h"

LG_IMPLEMENT_DYNCREATE(LightSource,RenderObject);

int LightSource::getNumSamples()const
{
	return -1;
}

void LightSource::getSamples(ShadingState&state)const
{
}

void LightSource::getPhoton(double randX1,double randY1,double randX2,double randY2,
					   Point3&p,Vector3&dir,Color&power)const
{
}

float LightSource::getPower()const
{
	return 0.0f;
}

Instance* LightSource::createInstance()
{
	return NULL;
}