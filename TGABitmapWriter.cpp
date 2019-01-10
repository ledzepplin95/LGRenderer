#include "StdAfx.h"

#include "TGABitmapWriter.h"
#include "ColorEncoder.h"
#include "Color.h"

LG_IMPLEMENT_DYNCREATE(TGABitmapWriter,BitmapWriter)

void TGABitmapWriter::configure(CString option,CString v) 
{
}

void TGABitmapWriter::openFile(CString fName) 
{
	fileName=fName;
}

void TGABitmapWriter::writeHeader(int w,int h,int tileSize)
{
	width=w;
	height=h;
	data.resize(width*height*4);
}

void TGABitmapWriter::writeTile(int x,int y,int w,int h, 
			  const vector<Color>&color,const vector<float>&alpha)
{
	vector<Color> c=ColorEncoder::unlinearize(color);
	vector<byte> tileData=ColorEncoder::quantizeRGBA8(c,alpha);
	for(int j=0, index=0; j<h; j++)
	{
		int imageIndex=4*(x+(height-1-(y+j))*width);
		for(int i=0; i<w; i++, index+=4, imageIndex+=4) 
		{			
			data[imageIndex+0]=tileData[index+2];
			data[imageIndex+1]=tileData[index+1];
			data[imageIndex+2]=tileData[index+0];
			data[imageIndex+3]=tileData[index+3];
		}
	}
}

void TGABitmapWriter::closeFile()
{
	CFile f;
	if( !f.Open(fileName,CFile::modeWrite | CFile::typeBinary) )
		return;

	byte tgaHeader[]={ 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	f.Write(&tgaHeader[0],sizeof(tgaHeader));
	int num=width&0xFF;
	f.Write(&num,sizeof(num));
	num=(width>>8)&0xFF;
	f.Write(&num,sizeof(num));
	num=height&0xFF;
	f.Write(&num,sizeof(num));
	num=(height>>8)&0xFF;
	f.Write(&num,sizeof(num));	
	num=32;
	f.Write(&num,sizeof(num));
	num=0;
	f.Write(&num,sizeof(num));	
	f.Write(&data[0],sizeof(byte)*data.size());

	f.Close();
}