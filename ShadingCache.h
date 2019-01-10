#pragma once

#include "color.h"

class ShadingCache
{
	class Sample 
	{
	public:
		Sample();
		
		Instance* i;
		Shader* s;
		float nx,ny,nz;
		float dx,dy,dz;
		Color c;
		Sample *next;

		Sample&operator=(const Sample&src);
	};

public:
    ShadingCache();
	~ShadingCache();
	ShadingCache(Sample*f,int d,long h,long m,long s,long n);
	ShadingCache(const ShadingCache&sc);

	void reset();
	BOOL lookup(const ShadingState&state,Shader*shader,Color&c);
    void add(const ShadingState&state,Shader*shader,Color&c);
	
	BOOL isNull()const;

private:
    Sample* first;
    int depth;

public:	
	long hits;
	long misses;
	long sumDepth;
	long numCaches;	
};
