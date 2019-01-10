#pragma once
#include "bitmap.h"

class GenericBitmap :
	public Bitmap
{
public:
	GenericBitmap(int width,int height);

	int getWidth()const;
	int getHeight()const;
	Color readColor(int x,int y)const;
	float readAlpha(int x,int y)const;

	void writePixel(int x,int y,const Color&c,float a);
	void save(CString fileName);
	
private:
	int w,h;
	vector<Color> color;
	vector<float> alpha;	
};
