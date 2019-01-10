#include "StdAfx.h"

#include "ProgressiveRenderer.h"
#include "timer.h"
#include "strutil.h"
#include "Scene.h"
#include "qmc.h"
#include "intersectionstate.h"
#include "ShadingState.h"
#include "shadingcache.h"
#include "display.h"
#include "threadthing.h"

LG_IMPLEMENT_DYNCREATE(ProgressiveRenderer,ImageSampler)

ProgressiveRenderer::SmallBucket::SmallBucket()
{
    x=y=size=0;
	contrast=0.0f;
}

ProgressiveRenderer::SmallBucket::SmallBucket(const ProgressiveRenderer::SmallBucket &o)
{
	if(this==&o) return;
    x=o.x;
	y=o.y;
	size=o.size;
	contrast=o.contrast;
}

int ProgressiveRenderer::SmallBucket::compareTo(const SmallBucket&o)
{
	if(contrast<o.contrast)
		return -1;
	if(contrast==o.contrast)
		return 0;

	return 1;
}

ProgressiveRenderer::SmallBucket& ProgressiveRenderer::SmallBucket::
    operator=(const SmallBucket&buck)
{
	if(this==&buck) return *this;

	x=buck.x;
	y=buck.y;
	size=buck.size;
	contrast=buck.contrast;

	return *this;
}

bool ProgressiveRenderer::cmp::operator()(const SmallBucket&a,const SmallBucket&b)
{
	return a.contrast>b.contrast;
}	

ProgressiveRenderer::ProgressiveRenderer()
{
	imageWidth=640;
	imageHeight=480;
}

BOOL ProgressiveRenderer::prepare(Options*o,Scene*s,int w,int h) 
{
	scene=s;
	imageWidth=w;
	imageHeight=h;
	
	return TRUE;
}

static CCriticalSection cs;

DWORD WINAPI pr_run(LPVOID lpParam) 
{
	ProgressiveRenderer*pr=(ProgressiveRenderer*)lpParam;	
	
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
			cs.Lock();
		int n=pr->progressiveRenderNext(iState);	
		if(pr->counter>=pr->counterMax)
		{
			cs.Unlock();
			return 1;
		}
		pr->counter+=n;
		
		pr->scene->accumulateStats(iState);
		cs.Unlock();
	}

	return 0;
}

void ProgressiveRenderer::render(Display*dis) 
{
	display=dis;
	display->imageBegin(imageWidth,imageHeight,0);
	
	SmallBucket b;
	b.x=b.y=0;
	int s=max(imageWidth,imageHeight);
	b.size=1;
	while(b.size<s)
		b.size<<=1;
	smallBucketQueue=priority_queue<SmallBucket,vector<SmallBucket>,cmp>();
	smallBucketQueue.push(b);
	StrUtil::PrintPrompt("Progressive Render",0,imageWidth*imageHeight);
	Timer t;
	t.start();
	counter=0;
	counterMax=imageWidth*imageHeight;

	threadCount=scene->getThreads();
	vector<HANDLE> thrds;
	thrds.resize(scene->getThreads());
	for(int i=0; i<scene->getThreads(); i++)
	{
		DWORD threadID=i;
		::SetThreadPriority(thrds[i],scene->getThreadPriority());
		thrds[i]=::CreateThread(NULL,0,pr_run,this,0,&threadID);
		while(thrds[i]==NULL)
		{
			thrds[i]=::CreateThread(NULL,0,pr_run,this,0,&threadID);
			if(thrds[i]!=NULL) break;
		}
	}
	::WaitForMultipleObjects(scene->getThreads(),&thrds[0],TRUE,INFINITE);
	for(int i=0; i<scene->getThreads(); i++)
		::CloseHandle(thrds[i]);
	thrds.clear();

	t.end();
	StrUtil::PrintPrompt("渲染线程时间: %s",t.toString());
	dis->imageEnd();
}

int ProgressiveRenderer::progressiveRenderNext(IntersectionState&iState)
{
	int TASK_SIZE=16;
	SmallBucket first=smallBucketQueue.top();	
	int ds=first.size/TASK_SIZE;
	BOOL useMask=!smallBucketQueue.empty();
	int mask=2*first.size/TASK_SIZE-1;
	int pixels=0;
	for(int i=0, y=first.y; i<TASK_SIZE&&y<imageHeight; i++, y+=ds) 
	{
		for(int j=0, x=first.x; j<TASK_SIZE&&x<imageWidth; j++, x+=ds)
		{			
			if(useMask && (x&mask)==0 && (y&mask)==0)
				continue;
			int instance=((x&((1<<QMC::MAX_SIGMA_ORDER)-1))<<QMC::MAX_SIGMA_ORDER)
				+QMC::sigma(y&((1<<QMC::MAX_SIGMA_ORDER)-1),QMC::MAX_SIGMA_ORDER);
			double time=QMC::halton(1,instance);
			double lensU=QMC::halton(2,instance);
			double lensV=QMC::halton(3,instance);
			ShadingState* state=scene->getRadiance(iState,x,imageHeight-1-y, 
	                          lensU,lensV,time,instance,4,ShadingCache());
			Color c=state->getResult();
			pixels++;			
			display->imageFill(x,y,min(ds,imageWidth-x),min(ds,imageHeight-y),c,1);
			delete state;
		}
	}

	if(first.size>=2*TASK_SIZE) 
	{		
		int size=first.size>>1;
		for(int i=0; i<2; i++)
		{
			if(first.y+i*size<imageHeight)
			{
				for(int j=0; j<2; j++) 
				{
					if(first.x+j*size<imageWidth)
					{
						SmallBucket b;
						b.x=first.x+j*size;
						b.y=first.y+i*size;
						b.size=size;
						b.contrast=1.0f/size;
						smallBucketQueue.push(b);
					}
				}
			}
		}
	}

	return pixels;
}