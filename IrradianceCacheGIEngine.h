#pragma once
#include "giengine.h"
#include "GlobalPhotonMapInterface.h"
#include "color.h"
#include "point3.h"

class IrradianceCacheGIEngine :
	public GIEngine
{
	LG_DECLARE_DYNCREATE(IrradianceCacheGIEngine);
private:

	class Sample 
	{
	public:
		float pix,piy,piz;
		float nix,niy,niz;
		float invR0;
		Color irr;
		Sample *next;

		Sample();
		Sample(const Point3&p,const Vector3&n);
		Sample(const Point3&p,const Vector3&n,float r0,const Color&i);
	};

	class Node 
	{
	public:
		vector<Node*> children;
		Sample* first;
		Point3 center;
		float sideLength;
		float halfSideLength;
		float quadSideLength;
		float tolerance;		
		float minSpacing;
		float maxSpacing;

		Node(const Point3&c,float sLength);
		~Node();

		BOOL isInside(const Point3&p)const;
		float find(Sample&x)const; 
	};

private:
	int samples;
	float tolerance;
	float invTolerance;
	float minSpacing;
	float maxSpacing;
	Node* root;	
	GlobalPhotonMapInterface* globalPhotonMap;

	void insert(const Point3&p,const Vector3&n,float r0,const Color&i)const;
    Color getIrradiance(const Point3&p,const Vector3&n)const;

public:
	IrradianceCacheGIEngine();
	~IrradianceCacheGIEngine();

	Color getGlobalRadiance(ShadingState&state)const;
	BOOL init(Options*options,Scene*scene);
	Color getIrradiance(ShadingState&tate,const Color&diffuseReflectance)const;
};
