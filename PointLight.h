#pragma once
#include "lightsource.h"
#include "Point3.h"
#include "color.h"

class PointLight :
	public LightSource
{
    LG_DECLARE_DYNCREATE(PointLight);

public:
    PointLight();

	BOOL update(ParameterList&pl,LGAPI&api);

	int getNumSamples()const;
	void getSamples(ShadingState&state)const;
	void getPhoton(double randX1,double randY1,double randX2,double randY2, 
		Point3&p,Vector3&dir,Color&c)const;
	float getPower()const;
	Instance* createInstance();

private:
	Point3 lightPoint;
	Color power;	
};
