#include "StdAfx.h"
#include "ColumnBucketOrder.h"

LG_IMPLEMENT_DYNCREATE(ColumnBucketOrder,BucketOrder)

vector<int> ColumnBucketOrder::getBucketSequence(int nbw,int nbh)const 
{
	vector<int> coords;
	coords.resize(2*nbw*nbh);
	for(int i=0; i<nbw*nbh; i++) 
	{
		int bx=i/nbh;
		int by=i%nbh;
		if( (bx&1)==1 )
			by=nbh-1-by;
		coords[2*i+0]=bx;
		coords[2*i+1]=by;
	}

	return coords;
}
