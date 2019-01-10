#pragma once

#include "RenderObject.h"

class LightSource : public RenderObject
{
	LG_DECLARE_DYNCREATE(LightSource);
public:
    virtual int getNumSamples()const;
	virtual void getSamples(ShadingState&state)const;
	virtual void getPhoton(double randX1,double randY1,double randX2,double randY2,
		Point3&p,Vector3&dir,Color&power)const;
    virtual float getPower()const;
	virtual Instance* createInstance();	
};
