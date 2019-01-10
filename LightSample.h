#pragma once

#include "Color.h"
#include "ray.h"

class LightSample
{
public:	
	LightSample();
	~LightSample()
	{
		next = NULL;
	}
	LightSample(const Ray&r,const Color&diff,const Color&spec,
		LightSample*n);
	LightSample(const LightSample&ls);
	
	void setShadowRay(const Ray&r);
	void traceShadow(ShadingState&state);
	void setRadiance(const Color&d,const Color&s);
	LightSample& operator=(const LightSample&sample);

	BOOL isValid()const;
	Ray getShadowRay()const;
	Color getDiffuseRadiance()const;
	Color getSpecularRadiance()const;	
	float dot(const Vector3&v)const;

	Ray shadowRay;	
	Color ldiff;
	Color lspec;
	LightSample* next;
};
