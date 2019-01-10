#include "StdAfx.h"

#include "PNGBitmapReader.h"
#include "ximage.h"
#include "FindCxImageFileType.h"
#include "BitmapRGBA8.h"
#include "color.h"

LG_IMPLEMENT_DYNCREATE(PNGBitmapReader,BitmapReader)

Bitmap* PNGBitmapReader::load(CString fileName,BOOL isLinear)const
{	
	CxImage ci(fileName,FindType(fileName));	
	int width=ci.GetWidth();
	int height=ci.GetHeight();
	vector<byte> pixels;
	pixels.resize(4*width*height);
	for(int y=0, index=0; y<height; y++) 
	{
		for(int x=0; x<width; x++, index+=4)
		{
			RGBQUAD argb=ci.GetPixelColor(x,height-1-y);
			pixels[index+0]=argb.rgbRed;
			pixels[index+1]=argb.rgbGreen;
			pixels[index+2]=argb.rgbBlue;
			pixels[index+3]=argb.rgbReserved;
		}
	}
	if(!isLinear)
	{
		for(int index=0; index<pixels.size(); index+=4)
		{
			pixels[index+0]=Color::NATIVE_SPACE.rgbToLinear(pixels[index+0]);
			pixels[index+1]=Color::NATIVE_SPACE.rgbToLinear(pixels[index+1]);
			pixels[index+2]=Color::NATIVE_SPACE.rgbToLinear(pixels[index+2]);
		}
	}

	return new BitmapRGBA8(width,height,pixels);
}