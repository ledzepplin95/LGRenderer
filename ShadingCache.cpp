#include "StdAfx.h"

#include "ShadingCache.h"
#include "ShadingState.h"

ShadingCache::Sample::Sample()
{	
	i=NULL;
	s=NULL;
	nx=ny=nz=0.0f;
	dx=dy=dz=0.0f;
	c.set(0,0,0);
	next=NULL;
}

ShadingCache::Sample& ShadingCache::Sample::operator=(const Sample&src)
{
	if(this==&src) return *this;

	i=src.i;
	s=src.s;
	nx=src.nx;
	ny=src.ny;
	nz=src.nz;
	dx=src.dx;
	dy=src.dy;
	dz=src.dz;
	c=src.c;
	next=src.next;

	return *this;
}

ShadingCache::ShadingCache() 
{
	reset();
	hits=0;
	misses=0;	
}

ShadingCache::~ShadingCache()
{
	Sample*p=NULL;
	Sample*q=NULL;
	if(first!=NULL)
	{
		p=first->next;
		while(p) 
		{
			q=p->next;
			delete p;
			if(q==NULL) break;
			p=q;
		}
		first->next=NULL;
		delete first;
		first=NULL;
	}
}

ShadingCache::ShadingCache(ShadingCache::Sample *f,
						   int d,long h,long m,long s,long n)
{
	first=f;
	depth=d;
	hits=h;
	misses=m;
	sumDepth=s;
    numCaches=n;
}

ShadingCache::ShadingCache(const ShadingCache &sc)
{
	if(this==&sc) return;
	first=sc.first;
	depth=sc.depth;
	hits=sc.hits;
	misses=sc.misses;
	sumDepth=sc.sumDepth;
	numCaches=sc.numCaches;
}

void ShadingCache::reset() 
{
	sumDepth+=depth;
	if(depth>0)
		numCaches++;
	first=NULL;
	depth=0;
}

BOOL ShadingCache::lookup(const ShadingState&state,Shader*shader,Color&c) 
{	
	if(state.getNormal()==Vector3()) 
	{
		c=Color();
		return FALSE;
	}
	for(Sample* s=first; s!=NULL; s=s->next) 
	{
		if(s->i!=state.getInstance())
			continue;
		if(s->s!=shader)
			continue;
		if(state.getRay().dot(s->dx,s->dy,s->dz)<0.999f)
			continue;
		if(state.getNormal().dot(s->nx,s->ny,s->nz)<0.99f)
			continue;		
		hits++;
		c=s->c;
		return TRUE;
	}
	misses++;

	return FALSE;
}

void ShadingCache::add(const ShadingState&state,Shader*shader,Color&c) 
{	
	if(state.getNormal()==Vector3()) 
	{
		c=Color();
		return;
	}
	depth++;
	Sample* s=new Sample();
	s->i=state.getInstance();
	s->s=shader;
	s->c=c;
	s->dx=state.getRay().dx;
	s->dy=state.getRay().dy;
	s->dz=state.getRay().dz;
	s->nx=state.getNormal().x;
	s->ny=state.getNormal().y;
	s->nz=state.getNormal().z;
	s->next=first;
	first=s;
}

BOOL ShadingCache::isNull()const
{
	return first==NULL;
}

