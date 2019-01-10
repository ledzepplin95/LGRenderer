#include "StdAfx.h"

#include "ImgPipeDisplay.h"
#include <iostream>
#include "MathUtil.h"
#include "color.h"

ImgPipeDisplay::ImgPipeDisplay() 
{
}

void ImgPipeDisplay::imageBegin(int w,int h,int bucketSize) 
{
	cs.Lock();
	ih=h;
	outputPacket(5,w,h,MathUtil::floatToRawIntBits(1.0f),0);
	std::cout<<flush;
	cs.Unlock();
}

void ImgPipeDisplay::imagePrepare(int x,int y,int w,int h,int id)
{
}

void ImgPipeDisplay::imageUpdate(int x,int y,int w,int h,
								 vector<Color>&data,vector<float>&alpha) 
{
	cs.Lock();
	int xl=x;
	int xh=x+w-1;
	int yl=ih-1-(y+h-1);
	int yh=ih-1-y;
	outputPacket(2,xl,xh,yl,yh);
	vector<byte> rgba;
	rgba.resize(4*(yh-yl+1)*(xh-xl+1));
	for(int j=0, idx=0; j<h; j++) 
	{
		for(int i=0; i<w; i++, idx+=4) 
		{
			int rgb=data[(h-j-1)*w+i].toNonLinear().toRGB();
			int cr=(rgb>>16)&0xFF;
			int cg=(rgb>>8)&0xFF;
			int cb=rgb&0xFF;
			rgba[idx+0]=(byte)(cr&0xFF);
			rgba[idx+1]=(byte)(cg&0xFF);
			rgba[idx+2]=(byte)(cb&0xFF);
			rgba[idx+3]=(byte)(0xFF);
		}
	}
	try
	{
		char buffer[100];
		for(int i=0; i<rgba.size(); i++)
		    std::cout<<itoa(rgba[i],buffer,10);
	} 
	catch(...)
	{		
	}
	cs.Unlock();
}

void ImgPipeDisplay::imageFill(int x,int y,int w,int h,Color&c,float alpha) 
{
	cs.Lock();
	int xl=x;
	int xh=x+w-1;
	int yl=ih-1-(y+h-1);
	int yh=ih-1-y;
	outputPacket(2,xl,xh,yl,yh);
	int rgb=c.toNonLinear().toRGB();
	int cr=(rgb>>16)&0xFF;
	int cg=(rgb>>8)&0xFF;
	int cb=rgb&0xFF;
	vector<byte> rgba;
	rgba.resize(4*(yh-yl+1)*(xh-xl+1));
	for(int j=0, idx=0; j<h; j++)
	{
		for (int i=0; i<w; i++, idx+=4)
		{
			rgba[idx+0]=(byte)(cr&0xFF);
			rgba[idx+1]=(byte)(cg&0xFF);
			rgba[idx+2]=(byte)(cb&0xFF);
			rgba[idx+3]=(byte)(0xFF);
		}
	}
	try 
	{
		char buffer[100];
		for(int i=0; i<rgba.size(); i++)
			std::cout<<itoa(rgba[i],buffer,10);
	} 
	catch(...)
	{		
	}
	cs.Unlock();
}

void ImgPipeDisplay::imageEnd() 
{
	cs.Lock();
	outputPacket(4,0,0,0,0);
	std::cout<<flush;
	cs.Unlock();
}

void ImgPipeDisplay::outputPacket(int type,int d0,int d1,int d2,int d3)
{
	outputInt32(type);
	outputInt32(d0);
	outputInt32(d1);
	outputInt32(d2);
	outputInt32(d3);
}

void ImgPipeDisplay::outputInt32(int i) 
{
	char buffer[100];
	int radix=10;
	std::cout<<itoa((i>>24)&0xFF,buffer,radix);
	std::cout<<itoa((i>>16)&0xFF,buffer,radix);
	std::cout<<itoa((i>>8)&0xFF,buffer,radix);
	std::cout<<itoa(i&0xFF,buffer,radix);
}