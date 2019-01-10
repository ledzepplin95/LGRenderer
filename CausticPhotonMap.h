#pragma once

#include "causticphotonmapinterface.h"
#include "BoundingBox.h"

class CausticPhotonMap :
	public CausticPhotonMapInterface
{
    LG_DECLARE_DYNCREATE(CausticPhotonMap);

private:

	class Photon
	{
	public:
		float x;
		float y;
		float z;
		short dir;
		int power;
		int flags;

		enum
		{
			SPLIT_X=0,
			SPLIT_Y,
			SPLIT_Z,
			SPLIT_MASK,
		};
	
		Photon(){}
        Photon(const Point3&p,const Vector3&d,const Color&c);

		void setSplitAxis(int axis);
		Photon& operator=(const Photon&p);

		float getCoord(int axis)const;
		float getDist1(float px,float py,float pz)const;
		float getDist2(float px,float py,float pz)const;        			
	};

	class NearestPhotons 
	{
		int _max;
		BOOL _gotHeap;

	public: 
		vector<float> dist2;
		vector<Photon> index;
	
		int found;
		float px,py,pz;

		NearestPhotons(const Point3&p,int n,float maxDist2);
		void reset(const Point3&p,float maxDist2);
		void checkAddNearest(const Photon&p);
		NearestPhotons& operator=(const NearestPhotons&np);	
	};

public:
    void prepare(Options*options,const BoundingBox&sceneBounds);
	void store(ShadingState&state,const Vector3&dir,
		const Color&power,const Color&diffuse);
	void init();

	void getSamples(ShadingState&state)const;
	BOOL allowDiffuseBounced()const;
	BOOL allowReflectionBounced()const;
	BOOL allowRefractionBounced()const;
	int numEmit()const;

private:
	vector<Photon> photonList;
	vector<Photon> photons;
	int storedPhotons;
	int halfStoredPhotons;
	int log2n;
	int gatherNum;
	float gatherRadius;
	BoundingBox bounds;
	float filterValue;
	float maxPower;
	float maxRadius;
	int nEmit;
	CCriticalSection cs;

	void locatePhotons(NearestPhotons&np)const;

	void balance();
	void balanceSegment(vector<Photon>&temp,int index,int start,int end);
	void swap(int i,int j);
};
