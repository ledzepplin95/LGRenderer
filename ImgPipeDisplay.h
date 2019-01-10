#pragma once
#include "display.h"

class ImgPipeDisplay :
	public Display
{
public:
	ImgPipeDisplay();
	virtual ~ImgPipeDisplay(){};

	void imageBegin(int w,int h,int bucketSize);
    void imagePrepare(int x,int y,int w,int h,int id);
	void imageUpdate(int x,int y,int w,int h,
		vector<Color>&data,vector<float>&alpha);
	void imageFill(int x,int y,int w,int h,Color&c,float alpha);
	void imageEnd();

private:
	int ih;
	CCriticalSection cs;

	void outputPacket(int type,int d0,int d1,int d2,int d3);
	void outputInt32(int i);	
};
