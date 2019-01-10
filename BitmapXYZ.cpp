#include "stdafx.h"

#include "BitmapXYZ.h"
#include "xyzcolor.h"
#include "color.h"

BitmapXYZ::BitmapXYZ(int width,int height,const vector<float>&d)
{
	w=width;
	h=height;
	data=d;
}

int BitmapXYZ::getWidth()const 
{
	return w;
}

int BitmapXYZ::getHeight()const
{
	return h;
}

Color BitmapXYZ::readColor(int x,int y) const
{
	int index=3*(x+y*w);
	Color c;
	Color::NATIVE_SPACE.convertXYZtoRGB(XYZColor(data[index],
		data[index+1],data[index+2]),c);
	c.mul(0.1f);

	return c;
}

float BitmapXYZ::readAlpha(int x,int y) const
{
	return 1.0f;
}