#include "stdafx.h"

#include "BitmapG8.h"
#include "color.h"

BitmapG8::BitmapG8(int width,int height,const vector<byte>&d)
{
	w=width;
	h=height;
	data=d;
}

int BitmapG8::getWidth() const
{
	return w;
}

int BitmapG8::getHeight() const
{
	return h;
}

Color BitmapG8::readColor(int x,int y) const
{
	return Color((data[x+y*w]&0xFF)*INV255);
}

float BitmapG8::readAlpha(int x,int y) const
{
	return 1.0f;
}
