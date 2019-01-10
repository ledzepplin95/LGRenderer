#include "StdAfx.h"

#include "BucketRenderer.h"
#include "ShadingState.h"
#include "strutil.h"
#include "timer.h"
#include "qmc.h"
#include "MathUtil.h"
#include "PluginRegistry.h"
#include "BoxFilter.h"
#include "BucketOrderFactory.h"
#include "genericbitmap.h"
#include "ShadingCache.h"
#include "Scene.h"
#include "display.h"
#include "options.h"
#include "threadthing.h"

LG_IMPLEMENT_DYNCREATE(BucketRenderer,ImageSampler)

BucketRenderer::ImageSample::ImageSample()
{
	rx=ry=0.0f;
	i=n=0;
	c.set(-1,-1,-1);
	alpha=1.0f;
	instance=NULL;
	shader=NULL;
    nx=ny=nz=0.0f;
}

BucketRenderer::ImageSample::ImageSample(const BucketRenderer::ImageSample&is)
{
	if(this==&is) return;

	rx=is.rx;
    ry=is.ry;
	i=is.i;
	n=is.n;
	c=is.c;
	alpha=is.alpha;
	instance=is.instance;
	shader=is.shader;
}

BucketRenderer::ImageSample::ImageSample(float xx,float yy,int ii) 
{
    set(xx,yy,ii);
}

void BucketRenderer::ImageSample::set(float xx,float yy,int ii)
{
	rx=xx;
	ry=yy;
	i=ii;
	n=0;	
	alpha=0.0f;	
	nx=ny=nz=1.0f;
}

void BucketRenderer::ImageSample::set(const ShadingState&state) 
{
	if(state.isNull())
		c=Color::BLACK;
	else
	{
		c=state.getResult();		
		shader=state.getShader();
		instance=state.getInstance();
		if(state.getNormal()!=Vector3())
		{
			nx=state.getNormal().x;
			ny=state.getNormal().y;
			nz=state.getNormal().z;
		}	

		alpha=instance==NULL?0.0f:1.0f;
	}

	n=1;
}

void BucketRenderer::ImageSample::add(const ShadingState&state)
{
	if(n==0)
		c=Color::BLACK;
	if(!state.isNull())
	{
		c.add(state.getResult());
		alpha+=1.0f;
	}	
	
	n++;
}

void BucketRenderer::ImageSample::scale(float s) 
{
	c.mul(s);
	alpha*=s;
}

BOOL BucketRenderer::ImageSample::processed()const
{
	return c!=Color(-1,-1,-1);
}

BOOL BucketRenderer::ImageSample::sampled() const
{
	return n>0;
}

BOOL BucketRenderer::ImageSample::isDifferent(const ImageSample&sample,float thresh) const
{
	if(instance!=sample.instance)
		return TRUE;
	if(shader!=sample.shader)
		return TRUE;
	if(Color::hasContrast(c,sample.c,thresh))
		return TRUE;
	if(fabs(alpha-sample.alpha)/(alpha+sample.alpha)>thresh)
		return TRUE;
	float dot=nx*sample.nx+ny*sample.ny+nz*sample.nz;

	return dot<0.9f;
}

BucketRenderer::ImageSample& BucketRenderer::ImageSample::operator =(
	const BucketRenderer::ImageSample &sample)
{
	if(this==&sample) return *this;

	rx=sample.rx;
	ry=sample.ry;
	i=sample.i;
	n=sample.n;
	c=sample.c;
	alpha=sample.alpha;
	instance=sample.instance;
	shader=sample.shader;
	nx=sample.nx;
	ny=sample.ny;
	nz=sample.nz;	

	return *this;
}

BucketRenderer::ImageSample BucketRenderer::ImageSample::bilerp(
				const ImageSample&i00,const ImageSample&i01,
				const ImageSample&i10,const ImageSample&i11, 
				float dx,float dy,ImageSample&result) 
{
	float k00=(1.0f-dx)*(1.0f-dy);
	float k01=(1.0f-dx)*dy;
	float k10=dx*(1.0f-dy);
	float k11=dx*dy;
	Color c00=i00.c;
	Color c01=i01.c;
	Color c10=i10.c;
	Color c11=i11.c;
	Color c=Color::mul(k00,c00);
	c.madd(k01,c01);
	c.madd(k10,c10);
	c.madd(k11,c11);
	result.c=c;
	result.alpha=k00*i00.alpha+k01*i01.alpha+k10*i10.alpha+k11*i11.alpha;

	return result;
}

BucketRenderer::BucketRenderer() 
{
	bucketSize=32;
	bucketOrderName="hilbert";
	displayAA=FALSE;
	contrastThreshold=0.1f;
	filterName="box";
	jitter=FALSE;
	dumpBuckets=FALSE;
	superSampling=0;

	scene=NULL;
	display=NULL;
	bucketOrder=NULL;
    filter=NULL;
	minAADepth=0;
	maxAADepth=0;
}

BOOL BucketRenderer::prepare(Options*o,Scene*s,int w,int h) 
{
	scene=s;
	imageWidth=w;
	imageHeight=h;

	bucketSize=o->getInt("bucket.size",bucketSize);
	bucketOrderName=o->getString("bucket.order",bucketOrderName);
	minAADepth=o->getInt("aa.min",minAADepth);
	maxAADepth=o->getInt("aa.max",maxAADepth);
	superSampling=o->getInt("aa.samples",superSampling);
	displayAA=o->getBoolean("aa.display",displayAA);
	jitter=o->getBoolean("aa.jitter",jitter);
	contrastThreshold=o->getFloat("aa.contrast",contrastThreshold);

	bucketSize=MathUtil::clamp(bucketSize,16,512);
	int numBucketsX=(imageWidth+bucketSize-1)/bucketSize;
	int numBucketsY=(imageHeight+bucketSize-1)/bucketSize;
	bucketOrder=BucketOrderFactory::create(bucketOrderName);
	bucketCoords=bucketOrder->getBucketSequence(numBucketsX,numBucketsY);
	
	minAADepth=MathUtil::clamp(minAADepth,-4,5);
	maxAADepth=MathUtil::clamp(maxAADepth,minAADepth,5);
	superSampling=MathUtil::clamp(superSampling,1,256);
	invSuperSampling=1.0/superSampling;	
	subPixelSize=(maxAADepth>0)?(1<<maxAADepth):1;
	minStepSize=maxAADepth>=0?1:1<<(-maxAADepth);
	if(minAADepth==maxAADepth)
		maxStepSize=minStepSize;
	else
		maxStepSize=minAADepth>0?1<<minAADepth:subPixelSize<<(-minAADepth);
	useJitter=jitter && maxAADepth>0;	
	contrastThreshold=MathUtil::clamp(contrastThreshold,0.0f,1.0f);
	thresh=contrastThreshold*pow(2.0f,minAADepth);
	
	filterName=o->getString("filter",filterName);
	filter=PluginRegistry::filterPlugins.createObject(filterName);	
	if(filter==NULL)
	{
		StrUtil::PrintPrompt("未知的过滤器类型:\"%s\" -缺省为box",filterName);		
		filterName="box";
		filter=PluginRegistry::filterPlugins.createObject(filterName);
	}
	fhs=filter->getSize()*0.5f;
	fs=(int)ceil(subPixelSize*(fhs-0.5f));
	sigmaOrder=min(QMC::MAX_SIGMA_ORDER,max(0,maxAADepth)+13); 
	sigmaLength=1<<sigmaOrder;
	StrUtil::PrintPrompt("Bucket renderer设置:");
	StrUtil::PrintPrompt("  * 分辨率:         %dx%d",imageWidth,imageHeight);
	StrUtil::PrintPrompt("  * bucket大小:     %d",bucketSize);
	StrUtil::PrintPrompt("  * bucket数量:     %dx%d",numBucketsX,numBucketsY);
	if(minAADepth!=maxAADepth)
		StrUtil::PrintPrompt("  * 反锯齿:         %s->%s(自适应)",aaDepthToString(minAADepth), 
		aaDepthToString(maxAADepth));
	else
		StrUtil::PrintPrompt("  * 反锯齿:         %s(固定)",aaDepthToString(minAADepth));
	StrUtil::PrintPrompt("  * 每个采样光线数: %d",superSampling);
	StrUtil::PrintPrompt("  * 亚像素抖动:     %s",useJitter?"开":(jitter?"自动关闭":"关闭"));
	StrUtil::PrintPrompt("  * 对比度阈值:     %.2f",contrastThreshold);
	StrUtil::PrintPrompt("  * 过滤器类型:     %s",filterName);
	StrUtil::PrintPrompt("  * 过滤器大小:     %.2f像素",filter->getSize());

	return TRUE;
}

CString BucketRenderer::aaDepthToString(int depth)const
{
	int pixelAA=(depth<0)?-(1<<(-depth)):(1<<depth);
	CString str;
	str.Format("%s%d采样",depth<0?"1/":"",pixelAA*pixelAA);

	return str;
}

typedef struct brThreadParameter
{
	BucketRenderer*br;
	int threadID;

	brThreadParameter()
	{
        br=NULL;
		threadID=0;
	};

    brThreadParameter(BucketRenderer*bu,int tid)
	{
		br=bu;
		threadID=tid;
	}
	void set(BucketRenderer*bu,int tid)
	{
		br=bu;
		threadID=tid;
	}
} *pbrThreadParameter;

static std::vector<brThreadParameter> brtps;
static CCriticalSection cs;

DWORD WINAPI br_run(LPVOID lpParam) 
{
	pbrThreadParameter para=(pbrThreadParameter)lpParam;
	
	while(TRUE) 
	{
		if(isExit)
		{
		    InterlockedDecrement((long*)&threadCount);
			return 1;
		}
		IntersectionState iState;
		int bx,by;
		cs.Lock();
		if(para->br->bucketCounter>=para->br->bucketCoords.size())	
		{
			cs.Unlock();			
			return 1;
	    }
		bx=para->br->bucketCoords[para->br->bucketCounter+0];
		by=para->br->bucketCoords[para->br->bucketCounter+1];
		para->br->bucketCounter+=2;		
		
		para->br->renderBucket(para->br->display,
			bx,by,para->threadID,iState);	
		para->br->scene->accumulateStats(iState);
		cs.Unlock();
	}

	return 0;
}

//http://localhost:8000/doc/sample.jpg
void BucketRenderer::render(Display*dis) 
{
    display=dis;
	display->imageBegin(imageWidth,imageHeight,bucketSize);	
	bucketCounter=0;	
	StrUtil::PrintPrompt("渲染%d个bucket",bucketCoords.size());
	Timer timer;
	timer.start();	

	threadCount=scene->getThreads();
	vector<HANDLE> thrds;
	thrds.resize(scene->getThreads());
	if(!brtps.empty()) brtps.clear();
	brtps.resize(scene->getThreads());   
	for(int i=0; i<scene->getThreads(); i++)
	{		
		DWORD threadID=i;        
		::SetThreadPriority(thrds[i],scene->getThreadPriority());
		brtps[i].set(this,threadID);
		thrds[i]=::CreateThread(NULL,0,br_run,&brtps[i],0,&threadID);
		while(thrds[i]==NULL)
		{
           thrds[i]=::CreateThread(NULL,0,br_run,&brtps[i],0,&threadID);
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

void BucketRenderer::renderBucket(Display*dis,int bx,int by,int threadID, 
						  IntersectionState&iState) 
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
	int sx0=x0*subPixelSize-fs;
	int sy0=y0*subPixelSize-fs;
	int sbw=bw*subPixelSize+fs*2;
	int sbh=bh*subPixelSize+fs*2;	
	sbw=(sbw+(maxStepSize-1))&(~(maxStepSize-1));
	sbh=(sbh+(maxStepSize-1))&(~(maxStepSize-1));
	if(maxStepSize>1)
	{
		sbw++;
		sbh++;
	}
	vector<ImageSample> samples;
	samples.resize(sbw*sbh);
	float invSubPixelSize=1.0f/subPixelSize;
	for(int y=0,index=0; y<sbh; y++) 
	{
		for(int x=0; x<sbw; x++,index++)
		{
			int sx=sx0+x;
			int sy=sy0+y;
			int j=sx&(sigmaLength-1);
			int k=sy&(sigmaLength-1);
			int i=(j<<sigmaOrder)+QMC::sigma(k,sigmaOrder);			
            
			float dx=useJitter?(float)QMC::halton(0,k):0.5f;
			float dy=useJitter?(float)QMC::halton(0,j):0.5f;
			float rx=(sx+dx)*invSubPixelSize;
			float ry=(sy+dy)*invSubPixelSize;
			ry=imageHeight-ry;
			samples[index].set(rx,ry,i);
		}
	}
	for(int x=0; x<sbw-1; x+=maxStepSize)	
		for(int y=0; y<sbh-1; y+=maxStepSize)
			refineSamples(sbw,x,y,maxStepSize,thresh,iState,samples);	

	if(dumpBuckets)
	{
		StrUtil::PrintPrompt("转储bucket[%d,%d]到文件...",bx,by);
		GenericBitmap bitmap(sbw,sbh);
		for(int y=sbh-1,index=0; y>=0; y--)
			for (int x=0; x<sbw; x++, index++)
				bitmap.writePixel(x,y,samples[index].c,samples[index].alpha);
		CString str;
		str.Format(_T("bucket_%04d_%04d.png"),bx,by);
		bitmap.save(str);
	}
	if(displayAA)
	{		
		float invArea=invSubPixelSize*invSubPixelSize;
		for(int y=0, index=0; y<bh; y++) 
		{
			for(int x=0; x<bw; x++, index++)
			{
				int sd=0;
				for(int i=0; i<subPixelSize; i++) 
				{
					for(int j=0; j<subPixelSize; j++) 
					{
						int sx=x*subPixelSize+fs+i;
						int sy=y*subPixelSize+fs+j;
						int s=sx+sy*sbw;
						sd+=samples[s].sampled()?1:0;
					}
				}
				bucketRGB[index].set(sd*invArea);
				bucketAlpha[index]=1.0f;
			}
		}
	} 
	else 
	{		
		float cy=imageHeight-(y0+0.5f);
		for(int y=0, index=0; y<bh; y++, cy--)
		{
			float cx=x0+0.5f;
			for(int x=0; x<bw; x++,index++,cx++) 
			{
				Color c=Color::BLACK;
				float a=0.0f;
				float weight=0.0f;
				for(int j=-fs, sy=y*subPixelSize; j<= fs; j++, sy++) 
				{
					for(int i=-fs,sx=x*subPixelSize, s=sx+sy*sbw; i<=fs; i++,sx++,s++) 
					{
						float dx=samples[s].rx-cx;
						if(fabs(dx)>fhs)
							continue;
						float dy=samples[s].ry-cy;
						if(fabs(dy)>fhs)
							continue;
						float f=filter->get(dx,dy);
						c.madd(f,samples[s].c);
						a+=f*samples[s].alpha;
						weight+=f;
					}
				}
				float invWeight=1.0f/weight;
				c.mul(invWeight);
				a*=invWeight;
				bucketRGB[index]=c;
				bucketAlpha[index]=a;
			}
		}
	}

	dis->imageUpdate(x0,y0,bw,bh,bucketRGB,bucketAlpha);	
}

void BucketRenderer::computeSubPixel(IntersectionState&iState,ImageSample&sample) const
{
	float x=sample.rx;
	float y=sample.ry;
	double q0=QMC::halton(1,sample.i);
	double q1=QMC::halton(2,sample.i);
	double q2=QMC::halton(3,sample.i);
	ShadingState*s=NULL;
	if(superSampling>1)
	{		
		s=scene->getRadiance(iState,x,y,q1,q2,q0,sample.i,4,ShadingCache());
		sample.add(*s);
		delete s;
		for(int i=1; i<superSampling; i++)
		{
			double time=QMC::mod1(q0+i*invSuperSampling);
			double lensU=QMC::mod1(q1+QMC::halton(0,i));
			double lensV=QMC::mod1(q2+QMC::halton(1,i));
			s=scene->getRadiance(iState,x,y,lensU,lensV,time,sample.i+i,4,ShadingCache());
			sample.add(*s);
			delete s;
		}
		sample.scale((float)invSuperSampling);
	} 
	else 
	{	
		s=scene->getRadiance(iState,x,y,q1,q2,q0,sample.i,4,ShadingCache());
		sample.set(*s);
		delete s;
	}
}

void BucketRenderer::refineSamples(int sbw,int x,int y,int stepSize,
				                   float thresh,IntersectionState&iState,
								   vector<ImageSample>&samples)const
{
	int dx=stepSize;
	int dy=stepSize*sbw;
	int i00=x+y*sbw;
	ImageSample& s00=samples[i00];
	ImageSample& s01=samples[i00+dy];
	ImageSample& s10=samples[i00+dx];
	ImageSample& s11=samples[i00+dx+dy];
	if(!s00.sampled())
		computeSubPixel(iState,s00);
	if(!s01.sampled())
		computeSubPixel(iState,s01);
	if(!s10.sampled())
		computeSubPixel(iState,s10);
	if(!s11.sampled())
		computeSubPixel(iState,s11);
	if(stepSize>minStepSize) 
	{
		if( s00.isDifferent(s01,thresh) 
			|| s00.isDifferent(s10,thresh) 
			|| s00.isDifferent(s11,thresh) 
			|| s01.isDifferent(s11,thresh) 
			|| s10.isDifferent(s11,thresh) 
			|| s01.isDifferent(s10,thresh))
		{
			stepSize>>=1;
			thresh*=2;
			refineSamples(sbw,x,y,stepSize,thresh,iState,samples);
			refineSamples(sbw,x+stepSize,y,stepSize,thresh,iState,samples);
			refineSamples(sbw,x,y+stepSize,stepSize,thresh,iState,samples);
			refineSamples(sbw,x+stepSize,y+stepSize,stepSize,thresh,iState,samples);
			return;
		}
	}
	
	float ds=1.0f/stepSize;
	for(int i=0; i<=stepSize; i++)
		for(int j=0; j<=stepSize; j++)
			if(!samples[x+i+(y+j)*sbw].processed())
				ImageSample::bilerp(s00,s01,s10,s11,
				i*ds,j*ds,samples[x+i+(y+j)*sbw]);
}