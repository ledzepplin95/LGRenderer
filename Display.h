#pragma once

class Display
{
public:
	Display(){};
	virtual ~Display(){};

	virtual void imageBegin(int w,int h,int bucketSize);
	virtual void imagePrepare(int x,int y,int w,int h,int id);	
	virtual void imageUpdate(int x,int y,int w,int h, 
		vector<Color>&data,vector<float>&alpha);
	virtual void imageFill(int x,int y,int w,int h,Color&c,float alpha);
	virtual void imageEnd();	
};
