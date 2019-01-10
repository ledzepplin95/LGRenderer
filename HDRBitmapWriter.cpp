#include "StdAfx.h"

#include "HDRBitmapWriter.h"
#include "ColorEncoder.h"

LG_IMPLEMENT_DYNCREATE(HDRBitmapWriter,BitmapWriter)

void HDRBitmapWriter::configure(CString option,CString v) 
{
}

void HDRBitmapWriter::openFile(CString fName) 
{
	fileName=fName;
}

void HDRBitmapWriter::writeHeader(int w,int h,int tileSize) 
{
	width=w;
	height=h;
	data.resize(width*height);
}

void HDRBitmapWriter::writeTile(int x,int y,int w,int h, 
			   const vector<Color>&color,const vector<float>&alpha) 
{
	vector<int> tileData=ColorEncoder::encodeRGBE(color);
	for(int j=0, index=0, pixel=x+y*width; j<h; j++, pixel+=width-w)
		for(int i=0; i<w; i++, index++, pixel++)
			data[pixel]=tileData[index];
}

void HDRBitmapWriter::closeFile()
{	
	CFile f;
	if(!f.Open(fileName,CFile::modeWrite | CFile::typeBinary))
		return;

	CString str;
	str="#?RGBE\n";
	f.Write(str,str.GetLength());
	str="FORMAT=32-bit_rle_rgbe\n\n";
	f.Write(str,str.GetLength());
	str.Format(_T("-Y %d +X %d\n"),height,width);
	f.Write(str,str.GetLength());
	for(int i=0; i<data.size(); i++)
	{
		int rgbe=data[i];
        int data;
		data=rgbe>>24;
		f.Write(&data,sizeof(data));
		data=rgbe>>16;
		f.Write(&data,sizeof(data));
		data=rgbe>>8;
		f.Write(&data,sizeof(data));
		f.Write(&rgbe,sizeof(rgbe));
	}

	f.Close();
}