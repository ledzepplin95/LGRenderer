#include "StdAfx.h"

#include "AmbientOcclusionGIEngine.h"
#include "lGdef.h"
#include "float.h"
#include "ShadingState.h"
#include "options.h"

LG_IMPLEMENT_DYNCREATE(AmbientOcclusionGIEngine,GIEngine)

Color AmbientOcclusionGIEngine::getGlobalRadiance(ShadingState&state)const 
{
	return Color::BLACK;
}

BOOL AmbientOcclusionGIEngine::init(Options*options,Scene*scene)
{
	bright=options->getColor("gi.ambocc.bright",Color::WHITE);
	dark=options->getColor("gi.ambocc.dark",Color::BLACK);
	samples=options->getInt("gi.ambocc.samples",32);
	maxDist=options->getFloat("gi.ambocc.maxdist",0);
	maxDist=(maxDist<=0.0f)?FLT_MAX:maxDist;

	return TRUE;
}

Color AmbientOcclusionGIEngine::getIrradiance(ShadingState&state,
											  const Color&diffuseReflectance)const 
{
	OrthoNormalBasis onb=state.getBasis();
	Vector3 w;
	Color result=Color::BLACK;
	for(int i=0; i<samples; i++) 
	{
		float xi=(float)state.getRandom(i,0,samples);
		float xj=(float)state.getRandom(i,1,samples);
		float phi=2.0f*LG_PI*xi;
		float cosPhi=(float)cos(phi);
		float sinPhi=(float)sin(phi);
		float sinTheta=(float)sqrt(xj);
		float cosTheta=(float)sqrt(1.0f-xj);
		w.x=cosPhi*sinTheta;
		w.y=sinPhi*sinTheta;
		w.z=cosTheta;
		onb.transform(w);
		Ray r(state.getPoint(),w);
		r.setMax(maxDist);
		result.add(Color::blend(bright,dark,state.traceShadow(r)));
	}

	return result.mul((float)LG_PI/samples);
}