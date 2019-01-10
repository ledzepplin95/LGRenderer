#pragma once
#include "bitmapwriter.h"

class CxImage;
class PNGBitmapWriter :
	public BitmapWriter
{
	LG_DECLARE_DYNCREATE(PNGBitmapWriter);

public:
	void configure(CString option,CString v);
	void openFile(CString fName);
    void writeHeader(int w,int h,int tileSize);
	void writeTile(int x,int y,int w,int h,
		const vector<Color>&color,const vector<float>&alpha);
	void closeFile();

private:
	CString fileName;
	CxImage* image;		
};
