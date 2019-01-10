#include "stdafx.h"
#include "BitmapGA8.h"
#include "color.h"

BitmapGA8::BitmapGA8(int width,int height,const vector<byte>&d)
{
	w=width;
	h=height;
	data=d;
}

int BitmapGA8::getWidth()const 
{
	return w;
}

int BitmapGA8::getHeight()const
{
	return h;
}

Color BitmapGA8::readColor(int x,int y) const
{
	return Color((data[2*(x+y*w)+0]&0xFF)*INV255);
}

float BitmapGA8::readAlpha(int x,int y) const
{
	return (data[2*(x+y*w)+1]&0xFF)*INV255;
}