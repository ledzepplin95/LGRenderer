#include "StdAfx.h"

#include "LightServer.h"
#include "ShadingCache.h"
#include "ShadingState.h"
#include "Scene.h"
#include "strutil.h"
#include "PluginRegistry.h"
#include "Timer.h"
#include "qmc.h"
#include "options.h"
#include "BoundingBox.h"
#include "Instance.h"
#include "vector3.h"

LightServer::LightServer(void)
{
	scene=NULL;
	causticPhotonMap=NULL;
	giEngine=NULL;
	shaderOverride=NULL;
}

LightServer::LightServer(Scene*s)
{
	set(s);
}

void LightServer::set(Scene*s)
{
	scene=s;
	lights.clear();	
	shaderOverridePhotons=FALSE;

	maxDiffuseDepth=1;
	maxReflectionDepth=4;
	maxRefractionDepth=4;
	photonCounter=0;

	causticPhotonMap=NULL;
	giEngine=NULL;
	shaderOverride=NULL;
}

void LightServer::setLights(const vector<LightSource*>&ls) 
{
	lights=ls;
}

Scene* LightServer::getScene() const
{
	return scene;
}

void LightServer::setShaderOverride(Shader*shader,BOOL photonOverride) 
{
	shaderOverride=shader;
	shaderOverridePhotons=photonOverride;
}

BOOL LightServer::build(Options*options) 
{	
	maxDiffuseDepth=options->getInt("depths.diffuse",maxDiffuseDepth);
	maxReflectionDepth=options->getInt("depths.reflection",maxReflectionDepth);
	maxRefractionDepth=options->getInt("depths.refraction",maxRefractionDepth);
	CString giEngineType=options->getString("gi.engine",CString());
	giEngine=PluginRegistry::giEnginePlugins.createObject(giEngineType,2);
	CString caustics=options->getString("caustics",CString());
	causticPhotonMap=PluginRegistry::causticPhotonMapPlugins.createObject(caustics,2);
	
	maxDiffuseDepth=max(0,maxDiffuseDepth);
	maxReflectionDepth=max(0,maxReflectionDepth);
	maxRefractionDepth=max(0,maxRefractionDepth);

	Timer t;
	t.start();	
	int numLightSamples=0;
	for(int i=0; i<lights.size(); i++)
		numLightSamples+=lights[i]->getNumSamples();
	if(giEngine!=NULL)
	{
		if(!giEngine->init(options,scene))
			return FALSE;
	}	
	if(!calculatePhotons(causticPhotonMap,"caustic",0,options))
		return FALSE;
	t.end();

	StrUtil::PrintPrompt("Light Server统计:");
	StrUtil::PrintPrompt("  * 发现光源:      %d",lights.size());
	StrUtil::PrintPrompt("  * 光采样:        %d",numLightSamples);
	StrUtil::PrintPrompt("  * 最大光线跟踪深度:");
	StrUtil::PrintPrompt("      - 散射       %d",maxDiffuseDepth);
	StrUtil::PrintPrompt("      - 反射       %d",maxReflectionDepth);
	StrUtil::PrintPrompt("      - 折射       %d",maxRefractionDepth);
	StrUtil::PrintPrompt("  * GI引擎         %s",giEngineType.IsEmpty()?"none":giEngineType);
	StrUtil::PrintPrompt("  * 焦散:          %s",caustics.IsEmpty()?"none":caustics);
	StrUtil::PrintPrompt("  * Shader覆盖:    %s",shaderOverride!=NULL?"是":"无");
	StrUtil::PrintPrompt("  * Photon覆盖:    %s",shaderOverridePhotons?"是":"无");
	StrUtil::PrintPrompt("  * 创建时间:      %s",t.toString());

	return TRUE;
}

void LightServer::showStats()const 
{
}

typedef struct lsThreadParameter
{
	LightServer*ls;
	int start;
	int end;
	int seed;
	float scale;
	PhotonStore*ps;
    const vector<float>&histogram;

	lsThreadParameter(LightServer*_ls,int _start,int _end,
		int _seed,float _scale,PhotonStore*_ps,
		const vector<float>&_hist) : histogram(_hist)
	{
		ls=_ls;
		start=_start;
		end=_end;
		seed=_seed;
		scale=_scale;
		ps=_ps;		
	}
} *plsThreadParameter;

static CCriticalSection cs;

DWORD WINAPI ls_run(LPVOID lpParam) 
{
	plsThreadParameter para=(plsThreadParameter)lpParam;	
	for(int i=para->start; i<para->end; i++) 
	{
		cs.Lock();
		IntersectionState iState;
		para->ls->ls_photonCounterIncrement();
		int qmcI=i+para->seed;
		double rand=QMC::halton(0,qmcI)*para->histogram[para->histogram.size()-1];
		int j=0;
		while(rand>=para->histogram[j]&&j<para->histogram.size())
			j++;		
		if(j==para->histogram.size())
			continue;

		double randX1=(j==0)?rand/para->histogram[0]:(rand-para->histogram[j])
			/(para->histogram[j]-para->histogram[j-1]);
		double randY1=QMC::halton(1,qmcI);
		double randX2=QMC::halton(2,qmcI);
		double randY2=QMC::halton(3,qmcI);
		Point3 pt;
		Vector3 dir;
		Color power;
		para->ls->lights[j]->getPhoton(randX1,randY1,randX2,randY2,pt,dir,power);
		power.mul(para->scale);		
		Ray r(pt,dir);	
		para->ls->ls_scene()->trace(r,iState);
		if(iState.hit())
		{			
			ShadingState* state=ShadingState::createPhotonState(r,&iState,qmcI,para->ps,para->ls);
			para->ls->shadePhoton(*state,power);
			delete state;
		}
		cs.Unlock();
	}

	return 0;
}

BOOL LightServer::calculatePhotons(PhotonStore*m,CString t,int seed,
								   Options*options)
{
	if(m==NULL) return TRUE;
	if(lights.size()==0)
	{
		StrUtil::PrintPrompt("不能跟踪%s光子，场景中没有光源",t);
		return FALSE;
	}
	vector<float> histogram;
	histogram.resize(lights.size());
	histogram[0]=lights[0]->getPower();
	for(int i=1; i<lights.size(); i++)
		histogram[i]=histogram[i-1]+lights[i]->getPower();
	StrUtil::PrintPrompt("跟踪%s光子...",t);
	m->prepare(options,scene->getBounds());
	int numEmittedPhotons=m->numEmit();
	if( numEmittedPhotons<=0 
		|| histogram[histogram.size()-1]<=0.0f )
	{
		StrUtil::PrintPrompt("允许光子贴图，但是没有要发射的%s光子",t);
		return FALSE;
	}
	StrUtil::PrintPrompt("跟踪%d个光子",numEmittedPhotons);
	Timer photonTimer;
	photonTimer.start();

	float scale=1.0f/numEmittedPhotons;
	int delta=numEmittedPhotons/scene->getThreads();
	photonCounter=0;
	vector<HANDLE> thrds;
	thrds.resize(scene->getThreads());
    for(int i=0; i<scene->getThreads(); i++)
	{
		DWORD threadID=i;
		int start=threadID*delta;
		int end=(threadID==(scene->getThreads()-1))?numEmittedPhotons:(threadID+1)*delta; 

		lsThreadParameter lstp(this,start,end,seed,scale,m,histogram);
		::SetThreadPriority(thrds[i],scene->getThreadPriority());
		thrds[i]=::CreateThread(NULL,0,ls_run,&lstp,0,&threadID);
        while(thrds[i]==NULL)
		{
            thrds[i]=::CreateThread(NULL,0,ls_run,&lstp,0,&threadID);
			if(thrds[i]!=NULL) break;
		}
	}
	::WaitForMultipleObjects(scene->getThreads(),&thrds[0],TRUE,INFINITE);
	for(int i=0; i<scene->getThreads(); i++)
		::CloseHandle(thrds[i]);	
	
	photonTimer.end();
	StrUtil::PrintPrompt("%s光子跟踪时间为%s",t,photonTimer.toString());
	m->init();

	return TRUE;
}

void LightServer::shadePhoton(ShadingState&state,Color&power)const 
{
	state.getInstance()->prepareShadingState(state);
	Shader* shader=getPhotonShader(state);	
	if(shader!=NULL)	
	    shader->scatterPhoton(state,power);	
}

void LightServer::traceDiffusePhoton(ShadingState&previous,Ray&r,Color&power)const 
{
	if( previous.getDiffuseDepth()>=maxDiffuseDepth )
		return;
	IntersectionState& iState=previous.ss_is();
	scene->trace(r,iState);
	if( previous.getIntersectionState()->hit() ) 
	{		
		ShadingState* state=ShadingState::createDiffuseBounceState(previous,r,0);
		shadePhoton(*state,power);
		delete state;
	}
}

void LightServer::traceReflectionPhoton(ShadingState&previous,Ray&r,Color&power)const 
{
	if( previous.getReflectionDepth()>=maxReflectionDepth )
		return;
	IntersectionState& iState=previous.ss_is();
	scene->trace(r,iState);
	if( previous.getIntersectionState()->hit() )
	{		
		ShadingState* state=ShadingState::createReflectionBounceState(previous,r,0);
		shadePhoton(*state,power);
		delete state;
	}
}

void LightServer::traceRefractionPhoton(ShadingState&previous,Ray&r,Color&power)const
{
	if( previous.getRefractionDepth()>=maxRefractionDepth )
		return;
	IntersectionState& iState=previous.ss_is();
	scene->trace(r,iState);
	if( previous.getIntersectionState()->hit() ) 
	{		
		ShadingState* state=ShadingState::createRefractionBounceState(previous,r,0);
		shadePhoton(*state,power);
		delete state;
	}	
}

Shader* LightServer::getShader(ShadingState&state)const 
{
	return  shaderOverride!=NULL?shaderOverride:state.getShader();	
}

Shader* LightServer::getPhotonShader(ShadingState&state)const 
{
	return  (shaderOverride!=NULL&&shaderOverridePhotons)
		?shaderOverride:state.getShader();  
}

ShadingState* LightServer::getRadiance(float rx,float ry,float time,int i,int d,Ray&r,
						 IntersectionState&iState,ShadingCache&cache)
{	
	iState.time=time;
	scene->trace(r,iState);	
	if(iState.hit())
	{			
		ShadingState* state=ShadingState::createState(
			&iState,rx,ry,time,r,i,d,this);
		state->getInstance()->prepareShadingState(*state);
		Shader* shader=getShader(*state);
		if(shader==NULL)
		{
			state->setResult(Color::BLACK);
			return state;
		}
		if(!cache.isNull())
		{
			Color c;		
			if(cache.lookup(*state,shader,c)) 
			{
				if(c!=Color())
				{
					state->setResult(c);
					return state;
				}			
			}	
		}			
		state->setResult(shader->getRadiance(*state));	
		if(!cache.isNull())
			cache.add(*state,shader,state->getResult());
		//checkNanInf(state.getResult());

		return state;
	} 
	else	
	{			
		return new ShadingState();
	}
}

void LightServer::checkNanInf(const Color&c)
{	
	if(c.isNan())
	    StrUtil::PrintPrompt("NaN着色采样!");
	else if(c.isInf())
		StrUtil::PrintPrompt("Inf着色采样!");
}

void LightServer::shadeBakeResult(ShadingState&state) const
{
	Shader*shader=getShader(state);
	if(shader!=NULL)
	    state.setResult(shader->getRadiance(state));
	else 
		state.setResult((Color::BLACK));
}

Color LightServer::shadeHit(ShadingState&state)const
{
	state.getInstance()->prepareShadingState(state);
	Shader* shader=getShader(state);

	return (shader!=NULL)?shader->getRadiance(state):Color::BLACK;	
}

Color LightServer::traceGlossy(ShadingState&previous,Ray&r,int i)const 
{	
	if( previous.getReflectionDepth()>=maxReflectionDepth 
		|| previous.getDiffuseDepth()>0 )
		return Color::BLACK;
	IntersectionState& iState=previous.ss_is();
	iState.numGlossyRays++;
	scene->trace(r,iState);

	if(iState.hit())
	{
		ShadingState*s=ShadingState::createGlossyBounceState(previous,r,i);
		Color c=shadeHit(*s);
		delete s;

		return c;
	}
	else return Color::BLACK;	
}

Color LightServer::traceReflection(ShadingState&previous,Ray&r,int i)const 
{
	if( previous.getReflectionDepth()>=maxReflectionDepth 
		|| previous.getDiffuseDepth()>0 )
		return Color::BLACK;
	IntersectionState& iState=previous.ss_is();
	iState.numReflectionRays++;
	scene->trace(r,iState);

	if(iState.hit())
	{
		ShadingState*s=ShadingState::createReflectionBounceState(previous,r,i);
		Color c=shadeHit(*s);
		delete s;

		return c;
	}
	else return Color::BLACK;	
}

Color LightServer::traceRefraction(ShadingState&previous,Ray&r,int i)const 
{
	if( previous.getRefractionDepth()>=maxRefractionDepth 
		|| previous.getDiffuseDepth()>0 )
		return Color::BLACK;
	IntersectionState& iState=previous.ss_is();
	iState.numRefractionRays++;
	scene->trace(r,iState);

	if(iState.hit())
	{
		ShadingState*s=ShadingState::createRefractionBounceState(previous,r,i);
		Color c=shadeHit(*s);
		delete s;
		 
		return c;
	}
	else return Color::BLACK;    
}

ShadingState* LightServer::traceFinalGather(ShadingState&previous,Ray&r,int i)const
{
	if( previous.getDiffuseDepth()>=maxDiffuseDepth )
		return new ShadingState();
	IntersectionState& iState=previous.ss_is();
	scene->trace(r,iState);

	return iState.hit()?
		ShadingState::createFinalGatherState(previous,r,i):new ShadingState();	
}

Color LightServer::getGlobalRadiance(ShadingState&state)const
{
	if(giEngine==NULL) return Color::BLACK;

	return giEngine->getGlobalRadiance(state);
}

Color LightServer::getIrradiance(ShadingState&state,const Color&diffuseReflectance)const 
{
	if( giEngine==NULL || state.getDiffuseDepth()>=maxDiffuseDepth )
		return Color::BLACK;

	return giEngine->getIrradiance(state,diffuseReflectance);
}

void LightServer::initLightSamples(ShadingState&state) const
{
	for(int i=0; i<lights.size(); i++)
		lights[i]->getSamples(state);
}

void LightServer::initCausticSamples(ShadingState&state) const
{
    if(causticPhotonMap!=NULL)
	    causticPhotonMap->getSamples(state);
}

int& LightServer::ls_photonCounter()
{
	return photonCounter;
}

Scene* LightServer::ls_scene()
{
	return scene;
} 

LightServer& LightServer::operator =(const LightServer &ls)
{
	if(this==&ls) return *this;

    lights=ls.lights;
    scene=ls.scene;
    shaderOverride=ls.shaderOverride;
	shaderOverridePhotons=ls.shaderOverridePhotons;
    maxDiffuseDepth=ls.maxDiffuseDepth;
	maxReflectionDepth=ls.maxReflectionDepth;
	maxRefractionDepth=ls.maxRefractionDepth;
	causticPhotonMap=ls.causticPhotonMap;
	giEngine=ls.giEngine;
	photonCounter=ls.photonCounter;

	return *this;
}

void LightServer::ls_photonCounterIncrement()
{
	InterlockedIncrement((long*)&photonCounter);
}