#pragma once
#include "bitmap.h"

class BitmapXYZ :
	public Bitmap
{
public:
    BitmapXYZ(int width,int height,const vector<float>&d);

	int getWidth()const;
	int getHeight()const;
	Color readColor(int x,int y)const;
    float readAlpha(int x,int y)const;

private:
	int w,h;
	vector<float> data;
};
