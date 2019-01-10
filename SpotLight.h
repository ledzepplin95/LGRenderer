#pragma once

#include "lightsource.h"
#include "point3.h"
#include "Vector3.h"
#include "color.h"

class SpotLight : public LightSource
{
	 LG_DECLARE_DYNCREATE(SpotLight);

public:
	SpotLight(void);

	BOOL update(ParameterList&pl,LGAPI&api);

	int getNumSamples()const;
	void getSamples(ShadingState&state)const;
	void getPhoton(double randX1,double randY1,double randX2,double randY2, 
		Point3&p,Vector3&dir,Color&c)const;
	float getPower()const;
	Instance* createInstance();

private:
    Point3 position;
    Vector3 direction;
	Color intensity;
    double minAngle;
	double maxAngle;
};
