#pragma once
#include "imagesampler.h"

class SimpleRenderer :
	public ImageSampler
{
	LG_DECLARE_DYNCREATE(SimpleRenderer);

public:
	BOOL prepare(Options*options,Scene*s,int w,int h);
	void render(Display*dis);
	void renderBucket(int bx,int by,IntersectionState&iState);

public:
	Scene*scene;
	Display*display;
	int imageWidth,imageHeight;
	int numBucketsX,numBucketsY;
	int bucketCounter,numBuckets;	
};

