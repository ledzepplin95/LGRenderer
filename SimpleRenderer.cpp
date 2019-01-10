#include "StdAfx.h"

#include "SimpleRenderer.h"
#include "timer.h"
#include "strutil.h"
#include "Scene.h"
#include "ShadingCache.h"
#include "shadingstate.h"
#include "intersectionstate.h"
#include "display.h"
#include "threadthing.h"

LG_IMPLEMENT_DYNCREATE(SimpleRenderer,ImageSampler)

BOOL SimpleRenderer::prepare(Options*options,Scene*s,int w,int h) 
{
	scene=s;
	imageWidth=w;
	imageHeight=h;
	numBucketsX=(imageWidth+31)>>5;
	numBucketsY=(imageHeight+31)>>5;
	numBuckets=numBucketsX*numBucketsY;

	return TRUE;
}

static CCriticalSection cs;

DWORD WINAPI sr_run(LPVOID lpParam) 
{
	SimpleRenderer*sr=(SimpleRenderer*)lpParam;		
	
	while(TRUE)
	{
		if(isExit)
		{
			cs.Lock();
			threadCount--;			
			cs.Unlock();
			return 1;
		}
		IntersectionState iState;
		int bx,by;
		cs.Lock();
		if(sr->bucketCounter>=sr->numBuckets)
		{
			cs.Unlock();
			return 1;
		}
		by=sr->bucketCounter/sr->numBucketsX;
		bx=sr->bucketCounter%sr->numBucketsX;
		InterlockedIncrement((long*)&sr->bucketCounter);		
		sr->renderBucket(bx,by,iState);	
		sr->scene->accumulateStats(iState);
		cs.Unlock();
	}

	return 0;
}

void SimpleRenderer::render(Display*dis) 
{
	display=dis;
	display->imageBegin(imageWidth,imageHeight,32);	
	bucketCounter=0;
	Timer timer;
	timer.start();
	
	threadCount=scene->getThreads();
	vector<HANDLE> thrds;
	thrds.resize(scene->getThreads());
	for(int i=0; i<scene->getThreads(); i++)
	{
		DWORD threadID=i;
		::SetThreadPriority(thrds[i],scene->getThreadPriority());
		thrds[i]=::CreateThread(NULL,0,sr_run,this,0,&threadID);
		while(thrds[i]==NULL)
		{
			thrds[i]=::CreateThread(NULL,0,sr_run,this,0,&threadID);
			if(thrds[i]!=NULL) break;
		}
	}
	::WaitForMultipleObjects(scene->getThreads(),&thrds[0],TRUE,INFINITE);
	for(int i=0; i<scene->getThreads(); i++)
		::CloseHandle(thrds[i]);
	thrds.clear();

	timer.end();
	StrUtil::PrintPrompt("渲染线程时间: %s",timer.toString());
	dis->imageEnd();
}

void SimpleRenderer::renderBucket(int bx,int by,IntersectionState&iState) 
{	
	int x0=bx*32;
	int y0=by*32;
	int bw=min(32,imageWidth-x0);
	int bh=min(32,imageHeight-y0);

	vector<Color> bucketRGB;
	bucketRGB.resize(bw*bh);
	vector<float> bucketAlpha;
	bucketAlpha.resize(bw*bh);
	for(int y=0, i=0; y<bh; y++) 
	{
		for(int x=0; x<bw; x++, i++) 
		{
			ShadingState* state=scene->getRadiance(iState,x0+x, 
				imageHeight-1-(y0+y),0.0,0.0,0.0,0,0,ShadingCache());			
			bucketRGB[i]=state->getResult();
			delete state;
			bucketAlpha[i]=1.0f;
		}
	}

	display->imageUpdate(x0,y0,bw,bh,bucketRGB,bucketAlpha);	
}