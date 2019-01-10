#pragma once
#include "display.h"

class FileDisplay :
	public Display
{
public:
	FileDisplay(BOOL saveImage);
	FileDisplay(CString fName);
	virtual ~FileDisplay();

	void imageBegin(int w,int h,int bucketSize);
	void imagePrepare(int x,int y,int w,int h,int id);
	void imageUpdate(int x,int y,int w,int h,
		vector<Color>&data,vector<float>&alpha);
    void imageFill(int x,int y,int w,int h,Color&c,float alpha);
	void imageEnd();

private:
	BitmapWriter* writer;
	CString fileName;	
};
