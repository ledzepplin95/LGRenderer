#include "StdAfx.h"

#include "PathTracingGIEngine.h"
#include "LGdef.h"
#include "strutil.h"
#include "ShadingState.h"
#include "shader.h"
#include "options.h"
#include "Instance.h"

LG_IMPLEMENT_DYNCREATE(PathTracingGIEngine,GIEngine)

BOOL PathTracingGIEngine::init(Options*options,Scene*scene) 
{
	samples=options->getInt("gi.path.samples",16);
	samples=max(0,samples);
	StrUtil::PrintPrompt("Path tracer…Ë÷√:");
	StrUtil::PrintPrompt("  * ≤…—˘: %d",samples);

	return TRUE;
}

Color PathTracingGIEngine::getIrradiance(ShadingState&state,const Color&diffuseReflectance)const
{
	if(samples<=0) return Color::BLACK;	
	Color irr=Color::BLACK;
	OrthoNormalBasis onb=state.getBasis();
	Vector3 w;
	int n=state.getDiffuseDepth()==0?samples:1;
	for(int i=0; i<n; i++) 
	{
		float xi=(float)state.getRandom(i,0,n);
		float xj=(float)state.getRandom(i,1,n);
		float phi=2.0f*xi*LG_PI;
		float cosPhi=(float)cos(phi);
		float sinPhi=(float)sin(phi);
		float sinTheta=(float)sqrt(xj);
		float cosTheta=(float)sqrt(1.0f-xj);
		w.x=cosPhi*sinTheta;
		w.y=sinPhi*sinTheta;
		w.z=cosTheta;
		onb.transform(w);
		ShadingState* temp=state.traceFinalGather(Ray(state.getPoint(),w),i);
		temp->getInstance()->prepareShadingState(*temp);			
		irr.add(temp->getShader()->getRadiance(*temp));
		delete temp;
	}
	irr.mul((float)LG_PI/n);

	return irr;
}

Color PathTracingGIEngine::getGlobalRadiance(ShadingState&state)const 
{
	return Color::BLACK;
}