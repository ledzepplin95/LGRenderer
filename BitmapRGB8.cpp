#include "stdafx.h"

#include "BitmapRGB8.h"
#include "color.h"

BitmapRGB8::BitmapRGB8(int width,int height,const vector<byte>&d) 
{
	w=width;
	h=height;
	data=d;
}

int BitmapRGB8::getWidth()const 
{
	return w;
}

int BitmapRGB8::getHeight()const
{
	return h;
}

Color BitmapRGB8::readColor(int x,int y)const
{
	int index=3*(x+y*w);
	float r=(data[index+0]&0xFF)*INV255;
	float g=(data[index+1]&0xFF)*INV255;
	float b=(data[index+2]&0xFF)*INV255;

	return Color(r,g,b);
}

float BitmapRGB8::readAlpha(int x,int y) const
{
	return 1.0f;
}