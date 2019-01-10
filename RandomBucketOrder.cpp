#include "StdAfx.h"
#include "RandomBucketOrder.h"

LG_IMPLEMENT_DYNCREATE(RandomBucketOrder,BucketOrder)

vector<int> RandomBucketOrder::getBucketSequence(int nbw,int nbh) const
{
	vector<int> coords;
	coords.resize(2*nbw*nbh);
	for(int i=0; i<nbw*nbh; i++) 
	{
		int by=i/nbw;
		int bx=i%nbw;
		if( (by&1)==1 )
			bx=nbw-1-bx;
		coords[2*i+0]=bx;
		coords[2*i+1]=by;
	}

	long seed=2463534242L;
	for(int i=0; i<coords.size(); i++) 
	{		
		seed=xorshift(seed);
		int src=mod((int)seed,nbw*nbh);
		seed=xorshift(seed);
		int dst=mod((int)seed,nbw*nbh);
		int tmp=coords[2*src+0];
		coords[2*src+0]=coords[2*dst+0];
		coords[2*dst+0]=tmp;
		tmp=coords[2*src+1];
		coords[2*src+1]=coords[2*dst+1];
		coords[2*dst+1]=tmp;
	}

	return coords;
}

int RandomBucketOrder::mod(int a,int b) const
{
	int m=a%b;

	return (m<0)?m+b:m;
}

long RandomBucketOrder::xorshift(long y) const
{
	y=y^(y<<13);
	y=y^(y>>17); 
	y=y^(y<<5);

	return y;
}
