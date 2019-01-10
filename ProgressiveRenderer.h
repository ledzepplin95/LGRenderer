#pragma once
#include "imagesampler.h"
#include "Comparable.h"
#include <queue>

class ProgressiveRenderer :
	public ImageSampler
{
    LG_DECLARE_DYNCREATE(ProgressiveRenderer);

private:
	class SmallBucket : public Comparable<SmallBucket> 
	{
	public:
		int x,y,size;
		float contrast;

		SmallBucket();
        SmallBucket(const SmallBucket&o);

	    int compareTo(const SmallBucket&o);
        SmallBucket& operator=(const SmallBucket&buck);
	};

	struct cmp
	{
		bool operator()(const SmallBucket&a,const SmallBucket&b);		
	};

public:
	ProgressiveRenderer();

    BOOL prepare(Options*o,Scene*s,int w,int h);
	void render(Display*dis);

public:
	Scene* scene;
    int imageWidth, imageHeight;
	priority_queue<SmallBucket,vector<SmallBucket>,cmp> smallBucketQueue;
	Display* display;
	int counter,counterMax;

	int progressiveRenderNext(IntersectionState&iState);	
};
