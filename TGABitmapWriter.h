#pragma once
#include "bitmapwriter.h"

class TGABitmapWriter :
	public BitmapWriter
{
	LG_DECLARE_DYNCREATE(TGABitmapWriter);

public:
	void configure(CString option,CString v);
	void openFile(CString fName);
	void writeHeader(int w,int h,int tileSize);
	void writeTile(int x,int y,int w,int h, 
	    const vector<Color>&color,const vector<float>&alpha);
	void closeFile();

private:
	CString fileName;
	int width,height;
	vector<byte> data;	
};
