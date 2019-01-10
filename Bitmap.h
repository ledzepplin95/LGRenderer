#pragma once

class Bitmap
{
protected:
	static float INV255;
	static float INV65535;

public:
	Bitmap();	

    virtual int getWidth()const;
	virtual int getHeight()const;
	virtual Color readColor(int x,int y)const;
	virtual float readAlpha(int x,int y)const;		
};
