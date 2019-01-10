#include "StdAfx.h"
#include "DiagonalBucketOrder.h"

LG_IMPLEMENT_DYNCREATE(DiagonalBucketOrder,BucketOrder)

vector<int> DiagonalBucketOrder::getBucketSequence(int nbw,int nbh)const
{
	vector<int> coords;
	coords.resize(2*nbw*nbh);
	int x=0,y=0,nx=1,ny=0;
	for(int i=0; i<nbw*nbh; i++) 
	{
		coords[2*i+0]=x;
		coords[2*i+1]=y;
		do 
		{
			if(y==ny)
			{
				y=0;
				x=nx;
				ny++;
				nx++;
			} 
			else
			{
				x--;
				y++;
			}
		} while( (y>=nbh || x>=nbw) && i!=(nbw*nbh-1) );
	}

	return coords;
}