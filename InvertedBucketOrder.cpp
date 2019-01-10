#include "StdAfx.h"
#include "InvertedBucketOrder.h"

LG_IMPLEMENT_DYNCREATE(InvertedBucketOrder,BucketOrder)

InvertedBucketOrder::InvertedBucketOrder(BucketOrder* o) 
{
	order=o;
}

vector<int> InvertedBucketOrder::getBucketSequence(int nbw,int nbh) const
{
	vector<int> coords=order->getBucketSequence(nbw,nbh);
	for(int i=0; i<coords.size()/2; i+=2)
	{
		int src=i;
		int dst=coords.size()-2-i;
		int tmp=coords[src+0];
		coords[src+0]=coords[dst+0];
		coords[dst+0]=tmp;
		tmp=coords[src+1];
		coords[src+1]=coords[dst+1];
		coords[dst+1]=tmp;
	}

	return coords;
}
