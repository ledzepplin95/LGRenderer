#pragma once
#include "bitmap.h"

class BitmapBlack :
	public Bitmap
{
public:
	int getWidth()const;
	int getHeight()const;
	Color readColor(int x,int y)const;
    float readAlpha(int x,int y)const;
};
