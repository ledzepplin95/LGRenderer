#include "StdAfx.h"

#include "Scene.h"
#include "strutil.h"
#include "AccelerationStructureFactory.h"
#include "MathUtil.h"
#include "ShadingState.h"
#include "ShadingCache.h"
#include "AccelerationStructure.h"
#include "options.h"
#include "instancelist.h"
#include "ImageSampler.h"
#include "boundingbox.h"
#include "Instance.h"
#include "LightSource.h"
#include "camera.h"
#include "lightserver.h"
#include "threadthing.h"

Scene::Scene() 
{
	lightServer=new LightServer();
	lightServer->set(this);
	accelType="auto";
	bakingViewDependent=FALSE;

	camera=NULL;
	imageWidth=640;
	imageHeight=480;
	threads=0;
	lowPriority=FALSE;
	rebuildAccel=TRUE;

	instanceList=NULL;
	infiniteInstanceList=NULL;
	intAccel=NULL;
    bakingInstance=NULL;
	bakingPrimitives=NULL;
	bakingAccel=NULL;

	infiniteInstanceList=new InstanceList();
	instanceList=new InstanceList();
}

Scene::~Scene()
{    
	if(lightServer) delete lightServer;
	if(infiniteInstanceList) delete infiniteInstanceList;
	if(instanceList) delete instanceList;
}

static int GetCPUNumber()
{
	SYSTEM_INFO si;	
	::GetSystemInfo(&si);

	return si.dwNumberOfProcessors*2;//超线程
}

int Scene::getThreads() const
{
	return threads<=0?GetCPUNumber():threads;
}

int Scene::getThreadPriority()const
{
	return lowPriority?THREAD_PRIORITY_LOWEST:THREAD_PRIORITY_NORMAL;
}

void Scene::setCamera(Camera*c) 
{
	camera=c;
}

Camera* Scene::getCamera()const 
{
	return camera;
}

void Scene::setInstanceLists(const vector<Instance*>&instances,const vector<Instance*>&infinites) 
{
	infiniteInstanceList->set(infinites);
	instanceList->set(instances);
	rebuildAccel=TRUE;
}

void Scene::setLightList(const vector<LightSource*>&lights) 
{
	lightServer->setLights(lights);
}

void Scene::setShaderOverride(Shader* shader,BOOL photonOverride) 
{
	lightServer->setShaderOverride(shader,photonOverride);
}

void Scene::setBakingInstance(Instance*instance) 
{
	bakingInstance=instance;
}

ShadingState* Scene::getRadiance(IntersectionState&iState,float rx,float ry,
						 double lensU,double lensV,double time,
						 int inst,int dim,ShadingCache&cache) const
{
	iState.numEyeRays++;
	float sceneTime=camera->getTime((float)time);
	
	if(bakingPrimitives==NULL)
	{		
		Ray r=camera->getRay(rx,ry,imageWidth,imageHeight,lensU,lensV,
			sceneTime);

		return !r.isNull()?
			lightServer->getRadiance(rx,ry,sceneTime,inst,dim,r,iState,
			cache):new ShadingState();	
	}
	else
	{	
		Ray r(rx/imageWidth,ry/imageHeight,-1,0,0,1);
		traceBake(r,iState);
		if(!iState.hit()) return new ShadingState();
		ShadingState* state=ShadingState::createState(&iState,rx,ry, 
			sceneTime,r,inst,dim,lightServer);
		bakingPrimitives->prepareShadingState(*state);
		if(bakingViewDependent)
			state->setRay(camera->getRay(state->getPoint(),sceneTime));
		else 
		{
			Point3 p=state->getPoint();
			Vector3 n=state->getNormal();			
			Ray incoming(p.x+n.x,p.y+n.y,p.z+n.z,-n.x,-n.y,-n.z);
			incoming.setMax(1);
			state->setRay(incoming);
		}
		lightServer->shadeBakeResult(*state);

		return state;	
	}
}

BoundingBox Scene::getBounds() const
{
	return instanceList->getWorldBounds(Matrix4());
}

void Scene::accumulateStats(const IntersectionState&state) 
{
	stats.accumulate(state);
}

void Scene::accumulateStats(const ShadingCache&cache) 
{
	stats.accumulate(cache);
}

void Scene::trace(Ray&r,IntersectionState&state)const
{
	state.numRays++;
	state.instance=NULL;
	state.current=NULL;
	for(int i=0; i<infiniteInstanceList->getNumPrimitives(); i++)
		infiniteInstanceList->intersectPrimitive(r,i,state);
	state.current=NULL;
	intAccel->intersect(r,state);	
}

Color Scene::traceShadow(Ray&r,IntersectionState&state)const 
{
	state.numShadowRays++;
	trace(r,state);

	return state.hit()?Color::WHITE:Color::BLACK;
}

void Scene::traceBake(Ray&r,IntersectionState&state) const
{	
	state.current=bakingInstance;
	state.instance=NULL;
	bakingAccel->intersect(r,state);
}

void Scene::createAreaLightInstances() 
{
	vector<Instance*> infiniteAreaLights;
	vector<Instance*> areaLights;	
	for(int i=0; i<lightServer->lights.size(); i++)
	{
		LightSource*ls=lightServer->lights[i];
		Instance*lightInstance=ls->createInstance();
		if(lightInstance!=NULL)
		{
			if(lightInstance->getBounds()==BoundingBox())			
		        infiniteAreaLights.push_back(lightInstance);			
			else		
				areaLights.push_back(lightInstance);			
		}	
	}

	if( !infiniteAreaLights.empty() && infiniteAreaLights.size()>0 )       
		infiniteInstanceList->addLightSourceInstances(infiniteAreaLights);	
	else
		infiniteInstanceList->clearLightSources();

	if( !areaLights.empty() && areaLights.size()>0 )      
		instanceList->addLightSourceInstances(areaLights);	
	else
		instanceList->clearLightSources();	

	rebuildAccel=TRUE;
}

void Scene::removeAreaLightInstances() 
{
	infiniteInstanceList->clearLightSources();
	instanceList->clearLightSources();
}

void Scene::render(Options*options,ImageSampler*sampler,Display*display)
{
	stats.reset();
	t.start();
	if(display==NULL) return;

    if(bakingInstance!=NULL)
	{
		StrUtil::PrintPrompt("创建光影贴图的图素...");
		bakingPrimitives=bakingInstance->getBakingPrimitives();
		int n=bakingPrimitives->getNumPrimitives();
		StrUtil::PrintPrompt("创建光影贴图的加速结构(%d个图素)...",n);
		bakingAccel=AccelerationStructureFactory::create("auto",n,TRUE);
		bakingAccel->build(bakingPrimitives);	
	}
	else
	{
		bakingPrimitives=NULL;
		bakingAccel=NULL;
	}	
	bakingViewDependent=options->getBoolean("baking.viewdep",bakingViewDependent);
	if( (bakingInstance!=NULL && bakingViewDependent && camera==NULL)
		|| (bakingInstance==NULL && camera==NULL) )
	{
		StrUtil::PrintPrompt("未发现相机");
		return;
	}

	threads=options->getInt("threads",0);
	lowPriority=options->getBoolean("threads.lowPriority",TRUE);
	imageWidth=options->getInt("resolutionX",640);
	imageHeight=options->getInt("resolutionY",480);	
	imageWidth=MathUtil::clamp(imageWidth,1,1<<14);
	imageHeight=MathUtil::clamp(imageHeight,1,1<<14);
	createAreaLightInstances();	
	long numPrimitives=0;
	for(int i=0; i<instanceList->getNumPrimitives(); i++)
		numPrimitives+=instanceList->getNumPrimitives(i);
	StrUtil::PrintPrompt("场景统计:");
	StrUtil::PrintPrompt("  * 无限instance:    %d",infiniteInstanceList->getNumPrimitives());
	StrUtil::PrintPrompt("  * Instance:		 %d",instanceList->getNumPrimitives());
	StrUtil::PrintPrompt("  * 图素:            %d",numPrimitives);
	CString accelName=options->getString("accel",CString());
	if(!accelName.IsEmpty()) 
	{
		rebuildAccel=rebuildAccel || !accelType==accelName;
		accelType=accelName;
	}
	StrUtil::PrintPrompt("  * Instance加速:    %s",accelType);
	if(rebuildAccel)
	{
		intAccel=AccelerationStructureFactory::create(accelType,instanceList->getNumPrimitives(),FALSE);
		intAccel->build(instanceList);
		rebuildAccel=FALSE;
	}
	StrUtil::PrintPrompt("  * 场景边界:        %s",getBounds().toString());
	StrUtil::PrintPrompt("  * 场景中心:        %s",getBounds().getCenter().toString());
	StrUtil::PrintPrompt("  * 场景直径:        %.2f",getBounds().getExtents().length());
	StrUtil::PrintPrompt("  * 光影贴图烘焙:    %s",bakingInstance!=NULL?(bakingViewDependent?"view":"ortho"):"关闭");
	if(sampler==NULL) return;
	if( !lightServer->build(options) ) return;

	StrUtil::PrintPrompt("渲染中...");	
	stats.setResolution(imageWidth,imageHeight);
	sampler->prepare(options,this,imageWidth,imageHeight);
	sampler->render(display);
	if(threadCount>0)
	{		
		stats.displayStats();
		lightServer->showStats();
		removeAreaLightInstances();	
		bakingPrimitives=NULL;
		bakingAccel=NULL;
		isDone=TRUE;		
	}
	t.end();
	StrUtil::PrintPrompt("渲染时间: %s",t.toString());	
	StrUtil::PrintPrompt("完成");
}

BOOL Scene::calculatePhotons(PhotonStore* m,CString t,int seed,Options*options) 
{
	return lightServer->calculatePhotons(m,t,seed,options);
}