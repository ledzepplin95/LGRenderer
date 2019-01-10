#pragma once

#include "Statistics.h"
#include "Timer.h"

class Scene
{
public:
	Scene(void);
	~Scene();
	
	int getThreads()const;
	int getThreadPriority()const;    
	Camera* getCamera()const;
	BoundingBox getBounds()const;
	void trace(Ray&r,IntersectionState&state)const;
	Color traceShadow(Ray&r,IntersectionState&state)const;
	void traceBake(Ray&r,IntersectionState&state)const;
	ShadingState* getRadiance(IntersectionState&iState,float rx,float ry,
		double lensU,double lensV,double time,
		int instance,int dim,ShadingCache&cache)const;	

	void accumulateStats(const IntersectionState&state);
	void accumulateStats(const ShadingCache&cache);
	void setCamera(Camera*c);
	void setInstanceLists(const vector<Instance*>&instances, 
		const vector<Instance*>&infinites);
	void setLightList(const vector<LightSource*>&lights);
	void setShaderOverride(Shader*shader,BOOL photonOverride);
	void setBakingInstance(Instance*instance);			
	void createAreaLightInstances();
	void removeAreaLightInstances();
	void render(Options*options,ImageSampler*sampler,Display*display);
    BOOL calculatePhotons(PhotonStore*m,CString t,int seed,
		Options*options);

private:
	LightServer* lightServer;
	InstanceList* instanceList;
	InstanceList* infiniteInstanceList;
	Camera* camera;
	AccelerationStructure* intAccel;
	CString accelType;
	Statistics stats;

	BOOL bakingViewDependent;
	Instance* bakingInstance;
	PrimitiveList* bakingPrimitives;
	AccelerationStructure* bakingAccel;

	BOOL rebuildAccel;
	int imageWidth;
	int imageHeight;	
	int threads;
	BOOL lowPriority;

	Timer t;
};
