#include "StdAfx.h"
#include "HilbertBucketOrder.h"

LG_IMPLEMENT_DYNCREATE(HilbertBucketOrder,BucketOrder)

vector<int> HilbertBucketOrder::getBucketSequence(int nbw,int nbh)const
{
	int hi=0;
	int hn=0;
	while( ( (1<<hn)<nbw || (1<<hn)<nbh ) && hn<16 )
		hn++; 
	int hN=1<<(2*hn); 
	int n=nbw*nbh;
	vector<int> coords;
	coords.resize(2*n); 
	for(int i=0; i<n; i++)
	{
		int hx,hy;
		do 
		{			
			int s=hi; 			
			int comp,swap,cs,t,sr;
			s=s|(0x55555555<<(2*hn)); 
			sr=(s>>1)&0x55555555; 
			cs=((s&0x55555555)+sr)^0x55555555;		
			cs=cs^(cs>>2);
			cs=cs^(cs>>4);
			cs=cs^(cs>>8);
			cs=cs^(cs>>16);
			swap=cs&0x55555555; 
			comp=(cs>>1)&0x55555555;
			t=(s&swap)^comp; 
			s=s^sr^t^(t<<1);			
			s=s&((1<<2*hn)-1);			
			t=(s^(s>>1))&0x22222222;
			s=s^t^(t<<1);
			t=(s^(s>>2))&0x0C0C0C0C;
			s=s^t^(t<<2);
			t=(s^(s>>4))&0x00F000F0;
			s=s^t^(t<<4);
			t=(s^(s>>8))&0x0000FF00;
			s=s^t^(t<<8);
			hx=s>>16; 
			hy=s&0xFFFF;
			hi++;
		} while( (hx>=nbw || hy>=nbh || hx<0 || hy<0) && hi<hN );
		coords[2*i+0]=hx;
		coords[2*i+1]=hy;
	}

	return coords;
}
