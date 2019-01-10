#include "StdAfx.h"

#include "MultipassRenderer.h"
#include "strutil.h"
#include "MathUtil.h"
#include "Timer.h"
#include "bucketorderfactory.h"
#include "Scene.h"
#include "qmc.h"
#include "IntersectionState.h"
#include "shadingcache.h"
#include "ShadingState.h"
#include "Bucketorder.h"
#include "display.h"
#include "options.h"
#include "threadthing.h"

LG_IMPLEMENT_DYNCREATE(MultipassRenderer,ImageSampler)

MultipassRenderer::MultipassRenderer()
{
	bucketSize=32;
	bucketOrderName="hilbert";
	numSamples=16;
	shadingCache=FALSE;
}

BOOL MultipassRenderer::prepare(Options*o,Scene*s,int w,int h) 
{
	scene=s;
	imageWidth=w;
	imageHeight=h;

	bucketSize=o->getInt("bucket.size",bucketSize);
	bucketOrderName=o->getString("bucket.order",bucketOrderName);
	numSamples=o->getInt("aa.samples",numSamples);
	shadingCache=o->getBoolean("aa.cache",shadingCache);
	
	bucketSize=MathUtil::clamp(bucketSize,16,512);
	int numBucketsX=(imageWidth+bucketSize-1)/bucketSize;
	int numBucketsY=(imageHeight+bucketSize-1)/bucketSize;
	bucketOrder=BucketOrderFactory::create(bucketOrderName);
	bucketCoords=bucketOrder->getBucketSequence(numBucketsX,numBucketsY);
	
	numSamples=max(1,numSamples);
	invNumSamples=1.0f/numSamples;

	StrUtil::PrintPrompt("Multipass renderer设置:");
	StrUtil::PrintPrompt("  * 分辨率:         %dx%d",imageWidth,imageHeight);
	StrUtil::PrintPrompt("  * bucket大小:     %d",bucketSize);
	StrUtil::PrintPrompt("  * bucket数量:     %dx%d",numBucketsX,numBucketsY);
	StrUtil::PrintPrompt("  * 采样/像素:      %d",numSamples);
	StrUtil::PrintPrompt("  * 着色缓存:       %s",shadingCache?"enabled":"disabled");
	
	return TRUE;
}

typedef struct mprThreadParameter
{
	MultipassRenderer*mr;
	int threadID;

	mprThreadParameter()
	{
		mr=NULL;
		threadID=0;
	}
	mprThreadParameter(MultipassRenderer*m,int tid)
	{
		mr=m;
		threadID=tid;
	}
	void set(MultipassRenderer*m,int tid)
	{
		mr=m;
		threadID=tid;
	}
} *pmprThreadParameter;

static std::vector<mprThreadParameter> mprtps;
static CCriticalSection cs;

DWORD WINAPI mpr_run(LPVOID lpParam)
{
	pmprThreadParameter para=(pmprThreadParameter)lpParam;	
	
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
		ShadingCache cache;
		int bx,by;
		cs.Lock();
		if(para->mr->bucketCounter>=para->mr->bucketCoords.size())
		{
			cs.Unlock();
			return 1;
		}
		bx=para->mr->bucketCoords[para->mr->bucketCounter+0];
		by=para->mr->bucketCoords[para->mr->bucketCounter+1];
		para->mr->bucketCounter+=2;
		
		para->mr->renderBucket(para->mr->display,
			bx,by,para->threadID,iState,cache);
		para->mr->scene->accumulateStats(iState);
		if(para->mr->shadingCache)
			para->mr->scene->accumulateStats(cache);
		cs.Unlock();
	}

	return 0;
}

void MultipassRenderer::render(Display*dis) 
{
	display=dis;
	display->imageBegin(imageWidth,imageHeight,bucketSize);
	bucketCounter=0;
	Timer timer;
	timer.start();
	StrUtil::PrintPrompt("渲染%d个bucket",bucketCoords.size());	
	
	threadCount=scene->getThreads();
	vector<HANDLE> thrds;
	thrds.resize(scene->getThreads());
	if(!mprtps.empty()) mprtps.clear();
	mprtps.resize(scene->getThreads());
	for(int i=0; i<scene->getThreads(); i++)
	{
		DWORD threadID=i;	
		mprtps[i].set(this,threadID);
		::SetThreadPriority(thrds[i],scene->getThreadPriority());
		thrds[i]=::CreateThread(NULL,0,mpr_run,&mprtps[i],0,&threadID);
		while(thrds[i]==NULL)
		{
			thrds[i]=::CreateThread(NULL,0,mpr_run,&mprtps[i],0,&threadID);
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

void MultipassRenderer::renderBucket(Display*dis,int bx,int by,int threadID, 
				  IntersectionState&iState,ShadingCache&cache) 
{
	int x0=bx*bucketSize;
	int y0=by*bucketSize;
	int bw=min(bucketSize,imageWidth-x0);
	int bh=min(bucketSize,imageHeight-y0);	
	dis->imagePrepare(x0,y0,bw,bh,threadID);

	vector<Color> bucketRGB;
	bucketRGB.resize(bw*bh);
	vector<float> bucketAlpha;
	bucketAlpha.resize(bw*bh);

	for(int y=0, i=0,cy=imageHeight-1-y0; y<bh; y++, cy--)
	{
		for(int x=0, cx=x0; x<bw; x++, i++, cx++) 
		{		
			Color c=Color::BLACK;
			float a=0.0f;
			int instance=((cx&((1<<QMC::MAX_SIGMA_ORDER)-1))<<QMC::MAX_SIGMA_ORDER)
				+QMC::sigma(cy&((1<<QMC::MAX_SIGMA_ORDER)-1),QMC::MAX_SIGMA_ORDER);
			double jitterX=QMC::halton(0,instance);
			double jitterY=QMC::halton(1,instance);
			double jitterT=QMC::halton(2,instance);
			double jitterU=QMC::halton(3,instance);
			double jitterV=QMC::halton(4,instance);
			for(int s=0; s<numSamples; s++) 
			{
				float rx=cx+0.5f+(float)warpCubic(QMC::mod1(jitterX+s*invNumSamples));
				float ry=cy+0.5f+(float)warpCubic(QMC::mod1(jitterY+QMC::halton(0,s)));
				double time=QMC::mod1(jitterT+QMC::halton(1,s));
				double lensU=QMC::mod1(jitterU+QMC::halton(2,s));
				double lensV=QMC::mod1(jitterV+QMC::halton(3,s));
				ShadingState* state=scene->getRadiance(iState,rx,ry,lensU,lensV, 
					time,instance+s,5,cache);
				c.add(state->getResult());
				delete state;
				a++;				
			}
			bucketRGB[i]=c.mul(invNumSamples);
			bucketAlpha[i]=a*invNumSamples;
			cache.reset();
		}
	}
	
	dis->imageUpdate(x0,y0,bw,bh,bucketRGB,bucketAlpha);
}

float MultipassRenderer::warpTent(float x) 
{
	if(x<0.5f)
		return -1.0f+sqrt(2.0*x);
	else
		return 1.0f-sqrt(2.0-2.0*x);
}

double MultipassRenderer::warpCubic(double x)
{
	if(x<(1.0/24.0))
		return qpow(24.0*x)-2.0;
	if(x<0.5)
		return distb1((24.0/11.0)*(x-(1.0/24.0)))-1.0;
	if(x<(23.0/24.0))
		return 1.0-distb1((24.0/11.0)*((23.0/24.0)-x));

	return 2.0-qpow(24.0*(1.0-x));
}

double MultipassRenderer::qpow(double x) 
{ 
	return sqrt(sqrt(x));
}

double MultipassRenderer::distb1(double x) 
{
	double u=x;
	for(int i=0; i<5; i++)
		u=(11.0*x+u*u*(6.0+u*(8.0-9.0*u)))/(4.0+12.0*u*(1+u*(1-u)));

	return u;
}