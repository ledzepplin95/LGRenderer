#include "StdAfx.h"
#include "RowBucketOrder.h"

LG_IMPLEMENT_DYNCREATE(RowBucketOrder,BucketOrder)

vector<int> RowBucketOrder::getBucketSequence(int nbw,int nbh)const
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

	return coords;
}