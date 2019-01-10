#pragma once
#include "imagesampler.h"

class MultipassRenderer :
	public ImageSampler
{
	LG_DECLARE_DYNCREATE(MultipassRenderer);

public:
	MultipassRenderer();

	BOOL prepare(Options*o,Scene*s,int w,int h);
	void render(Display*dis);

public:
	Scene* scene;
	Display* display;
	
	int imageWidth;
	int imageHeight;

	CString bucketOrderName;
	BucketOrder* bucketOrder;
	int bucketSize;
	int bucketCounter;
	vector<int> bucketCoords;

	int numSamples;
	float invNumSamples;
	BOOL shadingCache;

	void renderBucket(Display*dis,int bx,int by,int threadID, 
		IntersectionState&iState,ShadingCache&cache);	

	static float warpTent(float x);
	static double warpCubic(double x);
	static double qpow(double x);
	static double distb1(double x);	
};
