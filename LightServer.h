#pragma once

class CausticPhotonMapInterface;
class LightServer
{
public:
	LightServer(void);
	LightServer(Scene*s);

	Scene* getScene()const;
	void showStats()const;
	Shader* getShader(ShadingState&state)const;
	Shader* getPhotonShader(ShadingState&state)const;
	void shadeBakeResult(ShadingState&state)const;
	Color getGlobalRadiance(ShadingState&state)const;
	Color getIrradiance(ShadingState&state,const Color&diffuseReflectance)const;
	Color shadeHit(ShadingState&state)const;
	void shadePhoton(ShadingState&state,Color&power)const;
	void initLightSamples(ShadingState&state)const;
	void initCausticSamples(ShadingState&state)const;
	void traceDiffusePhoton(ShadingState&previous,Ray&r,Color&power)const ;
	void traceReflectionPhoton(ShadingState&previous,Ray&r,Color&power)const;
	void traceRefractionPhoton(ShadingState&previous,Ray&r,Color&power)const;		
	Color traceGlossy(ShadingState&previous,Ray&r,int i)const;
	Color traceReflection(ShadingState&previous,Ray&r,int i)const;
	Color traceRefraction(ShadingState&previous,Ray&r,int i)const;
	ShadingState* traceFinalGather(ShadingState&previous,Ray&r,int i)const;

	void set(Scene*s);
	void setLights(const vector<LightSource*>&ls);	
	void setShaderOverride(Shader*shader,BOOL photonOverride);
	BOOL build(Options*options);	
	BOOL calculatePhotons(PhotonStore*m,CString t,int seed,Options*options);
	ShadingState* getRadiance(float rx,float ry,float time,int i,int d,Ray&r,
		IntersectionState&iState,ShadingCache&cache);	
	Scene* ls_scene();
	int& ls_photonCounter();
	void ls_photonCounterIncrement();
	LightServer& operator=(const LightServer&ls);

	static void checkNanInf(const Color&c);

public:
	vector<LightSource*> lights;

private:
	friend Scene;
    Scene*scene;		
	Shader* shaderOverride;
	CausticPhotonMapInterface* causticPhotonMap;
	GIEngine* giEngine;
	
	BOOL shaderOverridePhotons;
	int maxDiffuseDepth;
	int maxReflectionDepth;
	int maxRefractionDepth;
	int photonCounter;
};
