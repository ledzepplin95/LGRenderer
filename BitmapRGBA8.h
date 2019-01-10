#pragma once
#include "bitmap.h"

class BitmapRGBA8 :
	public Bitmap
{
public:
	BitmapRGBA8(){}
    BitmapRGBA8(int width,int height,const vector<byte>&d);

	int getWidth()const;
	int getHeight()const;
	Color readColor(int x,int y)const;
	float readAlpha(int x,int y)const;
	
private:
	int w,h;
	vector<byte> data;
};
