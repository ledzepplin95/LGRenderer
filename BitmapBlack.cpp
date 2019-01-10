#include "stdafx.h"

#include "bitmapblack.h"
#include "color.h"

int BitmapBlack::getWidth()const 
{
	return 1;
}

int BitmapBlack::getHeight() const
{
	return 1;
}

Color BitmapBlack::readColor(int x,int y)const
{
	return Color::BLACK;
}

float BitmapBlack::readAlpha(int x,int y) const
{
	return 0.0f;
}