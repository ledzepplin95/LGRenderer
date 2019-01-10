#include "StdAfx.h"

#include "Statistics.h"
#include "ShadingCache.h"
#include "IntersectionState.h"
#include "strutil.h"

Statistics::Statistics() 
{
	reset();
}

void Statistics::reset() 
{
	numEyeRays=0;
	numShadowRays=0;
	numReflectionRays=0;
	numGlossyRays=0;
	numRefractionRays=0;
	numRays=0;
	numPixels=0;
	cacheHits=0;
	cacheMisses=0;
	cacheSumDepth=0;
	cacheNumCaches=0;
}

void Statistics::accumulate(const IntersectionState&state) 
{
	numEyeRays+=state.numEyeRays;
	numShadowRays+=state.numShadowRays;
	numReflectionRays+=state.numReflectionRays;
	numGlossyRays+=state.numGlossyRays;
	numRefractionRays+=state.numRefractionRays;
	numRays+=state.numRays;
}

void Statistics::accumulate(const ShadingCache&cache) 
{
	cacheHits+=cache.hits;
	cacheMisses+=cache.misses;
	cacheSumDepth+=cache.sumDepth;
	cacheNumCaches+=cache.numCaches;
}

void Statistics::setResolution(int w,int h) 
{
	numPixels=w*h;
}

void Statistics::displayStats() const
{	
	StrUtil::PrintPrompt("光线跟踪统计:");
	StrUtil::PrintPrompt("  * 已跟踪光线:                 (每像素)    (每根光线)     (百分比)", 
		numRays);
	printRayTypeStats("eye",numEyeRays);
	printRayTypeStats("shadow",numShadowRays);
	printRayTypeStats("reflection",numReflectionRays);
	printRayTypeStats("glossy",numGlossyRays);
	printRayTypeStats("refraction",numRefractionRays);
	printRayTypeStats("other",
		numRays-numEyeRays-numShadowRays-numReflectionRays
		-numGlossyRays-numRefractionRays);
	printRayTypeStats("总共",numRays);
	if(cacheHits+cacheMisses>0) 
	{
		StrUtil::PrintPrompt("着色缓存统计:");
		StrUtil::PrintPrompt("  * 查询:             %d",cacheHits+cacheMisses);
		StrUtil::PrintPrompt("  * 命中:             %d",cacheHits);
		StrUtil::PrintPrompt("  * 命中率:           %d%%", 
			(100*cacheHits)/(cacheHits+cacheMisses));
		StrUtil::PrintPrompt("  * 平均缓存深度:     %.2f", 
			(double)cacheSumDepth/(double)cacheNumCaches);
	}
}

void Statistics::printRayTypeStats(CString name,long n) const
{
	if(n>0)
	{
		double per1=(double)n/(double)numPixels;
		double per2=(double)n/(double)numEyeRays;
		double per3=(double)n/(double)(numRays*0.01);
		StrUtil::PrintPrompt("      %-10s  %11d   %7.2f      %7.2f      %6.2f%%",name,n,
		per1,per2,per3);
	}
}