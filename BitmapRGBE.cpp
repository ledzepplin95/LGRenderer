#include "stdafx.h"

#include "BitmapRGBE.h"
#include "color.h"

class vinitBitmapRGBE
{
public:
	vinitBitmapRGBE(void);		
};

float BitmapRGBE::EXPONENT[256]={0.0f};
static vinitBitmapRGBE ini;

vinitBitmapRGBE::vinitBitmapRGBE()
{
	BitmapRGBE::initial();
}

BitmapRGBE::BitmapRGBE(int width,int height,const vector<int>&d) 
{
	w=width;
	h=height;
	data=d;
}

int BitmapRGBE::getWidth()const 
{
	return w;
}

int BitmapRGBE::getHeight() const
{
	return h;
}

Color BitmapRGBE::readColor(int x,int y) const
{
	int rgbe=data[x+y*w];
	float f=EXPONENT[rgbe&0xFF];
	float r=f*((rgbe>>24)+0.5f);
	float g=f*(((rgbe>>16)&0xFF)+0.5f);
	float b=f*(((rgbe>>8)&0xFF)+0.5f);

	return Color(r,g,b);
}

float BitmapRGBE::readAlpha(int x,int y)const
{
	return 1.0f;
}

void BitmapRGBE::initial()
{
	EXPONENT[0]=0.0f;
	for(int i=1; i<256; i++)
	{
		float f=1.0f;
		int e=i-(128+8);
		if(e>0)
			for(int j=0; j<e; j++)
				f*=2.0f;
		else
			for(int j=0; j<-e; j++)
				f*=0.5f;
		EXPONENT[i]=f;
	}
}