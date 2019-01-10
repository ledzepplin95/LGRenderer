#pragma once

class Statistics
{
public:
	Statistics();

	void reset();
	void accumulate(const IntersectionState&state);
	void accumulate(const ShadingCache&cache);
	void setResolution(int w,int h);
	void displayStats()const;

private:
    void printRayTypeStats(CString name,long n)const;

private:
	long numEyeRays;
	long numShadowRays;
	long numReflectionRays;
	long numGlossyRays;
	long numRefractionRays;
	long numRays;
	long numPixels;
	long cacheHits;
	long cacheMisses;
	long cacheSumDepth;
	long cacheNumCaches;	
};
