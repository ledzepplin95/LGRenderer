#include "stdafx.h"

#include "BitmapRGBA8.h"
#include "color.h"

BitmapRGBA8::BitmapRGBA8(int width,int height,const vector<byte>&d) 
{
	w=width;
	h=height;
	data=d;
}

int BitmapRGBA8::getWidth() const
{
	return w;
}

int BitmapRGBA8::getHeight()const
{
	return h;
}

Color BitmapRGBA8::readColor(int x,int y)const
{
	int index=4*(x+y*w);
	float r=(data[index+0]&0xFF)*INV255;
	float g=(data[index+1]&0xFF)*INV255;
	float b=(data[index+2]&0xFF)*INV255;

	return Color(r,g,b);
}

float BitmapRGBA8::readAlpha(int x,int y) const
{
	return (data[4*(x+y*w)+3]&0xFF)*INV255;
}