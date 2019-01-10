#pragma once

#include "globalphotonmapinterface.h"
#include "BoundingBox.h"
#include "vector3.h"
#include "color.h"

class PhotonGroup : public LGObject
{
	LG_DECLARE_DYNCREATE(PhotonGroup);

public:
	int id;
	int count;
	Vector3 normal;
	Color flux;
	Color radiance;
	Color diffuse;
	PhotonGroup *next;

	PhotonGroup(){}
	PhotonGroup(int iden,const Vector3&n);
};

class GridPhotonMap :
	public GlobalPhotonMapInterface
{
    LG_DECLARE_DYNCREATE(GridPhotonMap);

public:
    GridPhotonMap();

	void prepare(Options*options,const BoundingBox&sceneBounds);
	int size();
	void store(ShadingState&state,const Vector3&dir,
		const Color&power,const Color&diffuse);
	void init();
	void precomputeRadiance(BOOL includeDirect,BOOL includeCaustics);

    Color getRadiance(const Point3&p,const Vector3&n);
	BOOL allowDiffuseBounced()const;
	BOOL allowReflectionBounced()const;
	BOOL allowRefractionBounced()const;
	int numEmit()const;

private:
	int numGather;
	float gatherRadius;
	int numStoredPhotons;
	int nx,ny,nz;
	BoundingBox bounds;
	vector<PhotonGroup*> cellHash;
	int hashSize;
	int hashPrime;	
	int nEmit;
	CCriticalSection cs;

	static float NORMAL_THRESHOLD;
	static int PRIMES[];

	void growPhotonHash();
	PhotonGroup* get(int x,int y,int z)const;
};
