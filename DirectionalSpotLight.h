#pragma once

#include "lightsource.h"
#include "point3.h"
#include "color.h"
#include "vector3.h"
#include "orthonormalbasis.h"

class DirectionalSpotLight :
	public LightSource
{
	LG_DECLARE_DYNCREATE(DirectionalSpotLight);

public:
	DirectionalSpotLight(void);
	
	BOOL update(ParameterList&pl,LGAPI&api);

    int getNumSamples()const;
	int getLowSamples()const;
	void getSamples(ShadingState&state)const;
	void getPhoton(double randX1,double randY1,double randX2,double randY2,
		Point3&p,Vector3&d,Color&power)const;
	float getPower()const;
	Instance* createInstance();

private:
	Point3 src;
	Vector3 dir;
	OrthoNormalBasis basis;
	float r,r2;
	Color radiance;
};
