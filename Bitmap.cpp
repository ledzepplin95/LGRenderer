#include "StdAfx.h"
#include "Bitmap.h"
#include "color.h"

float Bitmap::INV255=1.0f/255.0f;
float Bitmap::INV65535=1.0f/65535.0f;

Bitmap::Bitmap()
{
}

int Bitmap::getWidth()const
{
	return 0;
}

int Bitmap::getHeight()const
{
	return 0;
}

Color Bitmap::readColor(int x,int y)const
{
	return Color();
}

float Bitmap::readAlpha(int x,int y)const
{
	return 0.0f;
}
