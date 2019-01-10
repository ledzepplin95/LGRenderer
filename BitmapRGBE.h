#pragma once
#include "bitmap.h"

class BitmapRGBE :
	public Bitmap
{
public:
	BitmapRGBE(int width,int height,const vector<int>&d);

	int getWidth()const;
	int getHeight()const;
	Color readColor(int x,int y)const;
	float readAlpha(int x,int y)const;
	static void initial();

private:
	int w,h;
	vector<int> data;
	static float EXPONENT[256];	

};
