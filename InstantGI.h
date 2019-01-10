#pragma once
#include "giengine.h"
#include "PhotonStore.h"
#include "Vector3.h"
#include "color.h"
#include "point3.h"

class IGIPointLight 
{
public:
	IGIPointLight();
	IGIPointLight(const IGIPointLight&pl);

	Point3 p;
	Vector3 n;
	Color power;       

	IGIPointLight& operator=(const IGIPointLight&pl);
};

class PointLightStore : public PhotonStore
{
	LG_DECLARE_DYNCREATE(PointLightStore);
public:
	PointLightStore();
	PointLightStore(int n);

	vector<IGIPointLight> virtualLights;
	int nPhotons;
	CCriticalSection cs;

	void prepare(Options*options,const BoundingBox&sceneBounds);
	void store(ShadingState&state,const Vector3&dir,const Color&power,const Color&diffuse);
	void init();

	int numEmit()const;
	BOOL allowDiffuseBounced()const;
	BOOL allowReflectionBounced()const;
	BOOL allowRefractionBounced()const;	
};

class InstantGI :
	public GIEngine
{
     LG_DECLARE_DYNCREATE(InstantGI);
private:

	int numPhotons;
	int numSets;
	float c;
	int numBias;
	vector<vector<IGIPointLight>> virtualLights;   

public:
	Color getGlobalRadiance(ShadingState&state)const;
	BOOL init(Options*options,Scene*scene);
	Color getIrradiance(ShadingState&state,const Color&diffuseReflectance)const;	
};
