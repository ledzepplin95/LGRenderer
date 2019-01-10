#include "StdAfx.h"

#include "IGIBitmapWriter.h"
#include "MathUtil.h"
#include "xyzcolor.h"
#include "Color.h"

LG_IMPLEMENT_DYNCREATE(IGIBitmapWriter,BitmapWriter)

void IGIBitmapWriter::configure(CString option,CString v) 
{
}

void IGIBitmapWriter::openFile(CString fName)
{
	fileName=fName;
}

void IGIBitmapWriter::writeHeader(int w,int h,int tileSize) 
{
    width=w;
	height=h;
	xyz.resize(width*height*3);
}

void IGIBitmapWriter::writeTile(int x,int y,int w,int h,
			   const vector<Color>&color,const vector<float>&alpha) 
{
	for(int j=0, index=0,pixel=3*(x+y*width); 
		j<h; j++, pixel+=3*(width-w)) 
	{
		for(int i=0; i<w; i++, index++, pixel+=3) 
		{
			XYZColor c=Color::NATIVE_SPACE.convertRGBtoXYZ(color[index]);
			xyz[pixel+0]=c.getX();
			xyz[pixel+1]=c.getY();
			xyz[pixel+2]=c.getZ();
		}
	}
}

void IGIBitmapWriter::closeFile()
{
	CFile s;
	if( !s.Open(fileName,
		CFile::typeBinary | CFile::modeWrite) ) return;	

	write32(s,66613373);
	write32(s,1);
	write32(s,0); 
	write32(s,0);
	write32(s,width);
	write32(s,height);
	write32(s,1); 
	write32(s,0); 
	write32(s,width*height*12);
	write32(s,0); 
	int _size=5000;
	s.Write(&_size,sizeof(_size));
	for(int i=0; i<xyz.size(); i++)
		write32(s,xyz[i]);

	s.Close();
}

void IGIBitmapWriter::write32(CFile& stream,int i)
{
	CString str;
	str.Format(_T("%d"),i&0xFF);
	stream.Write(str,str.GetLength());
	str.Format(_T("%d"),(i>>8)&0xFF);
	stream.Write(str,str.GetLength());
	str.Format(_T("%d"),(i>>16)&0xFF);
	stream.Write(str,str.GetLength());
	str.Format(_T("%d"),(i>>24)&0xFF);
	stream.Write(str,str.GetLength());
}

void IGIBitmapWriter::write32(CFile& stream,float f) 
{
	write32(stream,MathUtil::floatToRawIntBits(f));
}