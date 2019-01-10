#include "stdafx.h"

#include "GenericBitmap.h"
#include "color.h"

GenericBitmap::GenericBitmap(int width,int height)
{
	w=width;
	h=height;
	color.resize(w*h);
	alpha.resize(w*h);
}

int GenericBitmap::getWidth()const 
{
	return w;
}

int GenericBitmap::getHeight()const
{
	return h;
}

Color GenericBitmap::readColor(int x,int y)const
{
	return color[x+y*w];
}

float GenericBitmap::readAlpha(int x,int y)const
{
	return alpha[x+y*w];
}

void GenericBitmap::writePixel(int x,int y,const Color&c,float a)
{
	color[x+y*w]=c;
	alpha[x+y*w]=a;
}

void GenericBitmap::save(CString fileName) 
{		
}