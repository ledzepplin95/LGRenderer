#pragma once
#include "bitmapwriter.h"

class IGIBitmapWriter :
	public BitmapWriter
{
	LG_DECLARE_DYNCREATE(IGIBitmapWriter);

public:

    void configure(CString option,CString v);
	void openFile(CString fName);
	void writeHeader(int w,int h,int tileSize);
	void writeTile(int x,int y,int w,int h,
		const vector<Color>&color,const vector<float>&alpha);
	void closeFile();
	static void write32(CFile& stream,int i);
	static void write32(CFile& stream,float f);

private:
	CString fileName;
	int width,height;
	vector<float> xyz;	
};
