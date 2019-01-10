#include "StdAfx.h"

#include "PNGBitmapWriter.h"
#include "FindCxImageFileType.h"
#include "ximage.h"
#include "Color.h"
#include "StrUtil.h"

LG_IMPLEMENT_DYNCREATE(PNGBitmapWriter,BitmapWriter)

void PNGBitmapWriter::configure(CString option,CString v)
{
}

void PNGBitmapWriter::openFile(CString fName) 
{
	fileName=fName;
	image=NULL;
}

void PNGBitmapWriter::writeHeader(int w,int h,int tileSize)
{
	image=new CxImage(w,h,32,FindType(fileName));    
}

void PNGBitmapWriter::writeTile(int x,int y,int w,int h,
					  const vector<Color>&color,const vector<float>&alpha)
{
	for(int j=0,index=0;j<h;j++)
	{
		for(int i=0;i<w;i++,index++)						
			image->SetPixelColor(x+i,image->GetHeight()-(y+j),
			color[index].copy().mul(1.0f/alpha[index]).toNonLinear().toRGBA(alpha[index]));		
	}
}

void PNGBitmapWriter::closeFile()
{
	image->Save(fileName,FindType(fileName));
	delete image;
	image=NULL;
}