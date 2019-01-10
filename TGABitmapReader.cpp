#include "StdAfx.h"

#include "TGABitmapReader.h"
#include "BitmapRGBA8.h"
#include "BitmapG8.h"
#include "BitmapRGB8.h"
#include "strutil.h"
#include "color.h"

int TGABitmapReader::CHANNEL_INDEX[]={2,1,0,3};

LG_IMPLEMENT_DYNCREATE(TGABitmapReader,BitmapReader)

Bitmap* TGABitmapReader::load(CString fileName,BOOL isLinear)const
{
	CFile f;
	if( !f.Open(fileName,CFile::modeRead | CFile::typeBinary) )
		return NULL;
	vector<byte> read;
	read.resize(4);
	
	byte data;
	f.Read(&data,sizeof(data));
	int idsize=data;
	f.Read(&data,sizeof(byte));
	int cmaptype=data; 
	if(cmaptype!=0)
	{
		StrUtil::PrintPrompt("不支持颜色映射 (类型: %d)",cmaptype);
		return NULL;
	}

	f.Read(&data,sizeof(data));
	int datatype=data;
	f.Read(&data,sizeof(data));
	f.Read(&data,sizeof(data));
	f.Read(&data,sizeof(data));
	f.Read(&data,sizeof(data));
	f.Read(&data,sizeof(data));
	f.Read(&data,sizeof(data)); 
	f.Read(&data,sizeof(data));
	f.Read(&data,sizeof(data));
	f.Read(&data,sizeof(data));
	
	int width;
	f.Read(&data,sizeof(data));
	width=data;
	f.Read(&data,sizeof(data));
	width|=data<<8;
	int height;
	f.Read(&data,sizeof(data));
	height=data;
	f.Read(&data,sizeof(data));
	height|=data<<8;

	int bits;
	f.Read(&data,sizeof(data));
	bits=data;
	int bpp=bits/8;
	f.Read(&data,sizeof(data));
	int imgdscr=data;	
	if(idsize!=0)
		f.Seek(idsize,CFile::current);
	
	vector<byte> pixels;
	pixels.resize(width*height*bpp);
	if( datatype==2 || datatype==3 ) 
	{
		if( bpp!=1 && bpp!=3 && bpp!=4 )
		{
			StrUtil::PrintPrompt("未压缩TGA的无效位深度 : %d",bits);
			return NULL;
		}
		for(int ptr=0; ptr<pixels.size(); ptr+=bpp)
	   {		
			f.Read(&read[0],bpp);
			for(int i=0; i<bpp; i++)
				pixels[ptr+CHANNEL_INDEX[i]]=read[i];
		}
	} 
    else if(datatype==10) 
	{
		if( bpp!=3 && bpp!=4 )
			StrUtil::PrintPrompt("行程编码TGA的无效位深度: %d",bits);
		for(int ptr=0; ptr<pixels.size();)
	   {
		    f.Read(&data,sizeof(data));
			int rle=data;
			int num=1+(rle&0x7F);
		    if( (rle&0x80)!=0 ) 
	       {				
				f.Read(&read[0],bpp);
				for(int j=0; j<num; j++) 
	           {
					for(int i=0; i<bpp; i++)
						pixels[ptr+CHANNEL_INDEX[i]]=read[i];
					ptr+=bpp;
				}
			} 
			else 
			{				
				for(int j=0; j<num; j++)
				{
					f.Read(&read[0],bpp);
					for(int i=0; i<bpp; i++)
						pixels[ptr+CHANNEL_INDEX[i]]=read[i];
					ptr+=bpp;
				}
			}
		}
	} 
	else
	{
		StrUtil::PrintPrompt("不支持的TGA图像类型: %d",datatype);
		return NULL;
	}

	if(!isLinear)
	{		
		for(int ptr=0; ptr<pixels.size(); ptr+=bpp) 
		{
			for(int i=0; i<3&&i<bpp; i++)
				pixels[ptr+i]=Color::NATIVE_SPACE.rgbToLinear(pixels[ptr+i]);
		}
	}

	if( (imgdscr&32)==32 )
	{
		for(int y=0, pix_ptr=0; y<(height/2); y++) 
		{
			int bot_ptr=bpp*(height-y-1)*width;
			for(int x=0; x<width; x++)
			{
				for(int i=0; i<bpp; i++) 
				{
					byte t=pixels[pix_ptr+i];
					pixels[pix_ptr+i]=pixels[bot_ptr+i];
					pixels[bot_ptr+i]=t;
				}
				pix_ptr+=bpp;
				bot_ptr+=bpp;
			}
		}

	}
	f.Close();

	switch(bpp)
	{
	case 1:
		return new BitmapG8(width,height,pixels);
	case 3:
		return new BitmapRGB8(width,height,pixels);
	case 4:
		return new BitmapRGBA8(width,height,pixels);
	}
	StrUtil::PrintPrompt("TGA读程序编码不一致");

	return NULL;
}