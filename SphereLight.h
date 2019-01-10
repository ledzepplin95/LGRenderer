#pragma once

#include "LightSource.h"
#include "shader.h"
#include "color.h"
#include "point3.h"

class SphereLight :
	public LightSource, public Shader
{
	LG_DECLARE_DYNCREATE(SphereLight);

public:
    SphereLight();

	BOOL update(ParameterList&pl,LGAPI&api);

	int getNumSamples()const;
	int getLowSamples()const;
	BOOL isVisible(ShadingState&state)const;
	void getSamples(ShadingState&state)const;
	void getPhoton(double randX1,double randY1,double randX2,double randY2,
		Point3&p,Vector3&dir,Color&power)const;
	float getPower()const;
	Instance* createInstance();
	Color getRadiance(ShadingState&state)const;
	void scatterPhoton(ShadingState&state,Color&power)const;	

private:
	Color radiance;
	int numSamples;
	Point3 center;
	float radius;
	float r2;
};
