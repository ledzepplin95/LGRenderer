#include "StdAfx.h"

#include "FakeGIEngine.h"
#include "LGdef.h"
#include "ShadingState.h"
#include "options.h"

LG_IMPLEMENT_DYNCREATE(FakeGIEngine,GIEngine)

Color FakeGIEngine::getIrradiance(ShadingState&state,const Color&diffuseReflectance)const 
{
	float cosTheta=Vector3::dot(up,state.getNormal());
	float sin2=1.0f-cosTheta*cosTheta;
	float sine=sin2>0.0f?(float)sqrt(sin2)*0.5f:0.0f;
	if(cosTheta>0.0f)
		return Color::blend(sky,ground,sine);
	else
		return Color::blend(ground,sky,sine);
}

Color FakeGIEngine::getGlobalRadiance(ShadingState&state) const
{
	return Color::BLACK;
}

BOOL FakeGIEngine::init(Options*options,Scene*scene) 
{
	up=options->getVector("gi.fake.up",Vector3(0,1,0)).normalize();
	sky=options->getColor("gi.fake.sky",Color::WHITE).copy();
	ground=options->getColor("gi.fake.ground",Color::BLACK).copy();
	sky.mul((float)LG_PI);
	ground.mul((float)LG_PI);

	return TRUE;
}