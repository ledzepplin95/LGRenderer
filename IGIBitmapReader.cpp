#include "StdAfx.h"

#include "IGIBitmapReader.h"
#include "BitmapXYZ.h"
#include "MathUtil.h"
#include "strutil.h"

LG_IMPLEMENT_DYNCREATE(IGIBitmapReader,BitmapReader)

Bitmap* IGIBitmapReader::load(CString fileName,BOOL isLinear)const
{
	CFile s;
	if(!s.Open(fileName,CFile::modeRead | CFile::typeBinary))
		return NULL;

	int magic=read32i(s);
	int version=read32i(s);
	s.Seek(8,CFile::current); 
	int width;
	width=read32i(s);
	int height;
	height=read32i(s);
	int superSample=read32i(s); 
	int compression=read32i(s);
	int dataSize=read32i(s);
	int colorSpace=read32i(s);
	s.Seek(5000,CFile::current);	
	if(magic!=66613373)
	{
		StrUtil::PrintPrompt("错误的magic: %d",magic);
		return NULL;
	}
	if(version!=1)
	{
		StrUtil::PrintPrompt("不支持的版本: %d",version);
        return NULL;
	}
	if(compression!=0)
	{
		StrUtil::PrintPrompt("不支持的压缩: %d",compression);
        return NULL;
	}
	if(colorSpace!=0)
	{
		StrUtil::PrintPrompt("不支持的颜色空间: %d",colorSpace);
        return NULL;
	}
	if(dataSize!=(width*height*12))
	{
		StrUtil::PrintPrompt("无效的数据块大小: %d",dataSize);
        return NULL;
	}
	if( width<=0 || height<=0 )
	{
		StrUtil::PrintPrompt("无效的图像大小: %d x %d",width,height);
        return NULL;
	}
	if(superSample<=0)
	{
		StrUtil::PrintPrompt("无效的超采样因子: %d",superSample);
        return NULL;
	}
	if( (width%superSample)!=0 || (height%superSample)!=0 )
	{
		StrUtil::PrintPrompt("无效的图像大小: %dx%d",width,height);
        return NULL;
	}

	vector<float> xyz;
	xyz.resize(width*height*3);
	for(int y=0, i=3*(height-1)*width; y<height; y++, i-=6*width)
	{
		for(int x=0; x<width; x++, i+=3) 
		{
			xyz[i+0]=read32f(s);
			xyz[i+1]=read32f(s);
			xyz[i+2]=read32f(s);
		}
	}
	s.Close();

	if(superSample>1) 
	{		
		vector<float> rescaled;
		rescaled.resize(xyz.size()/(superSample*superSample));
		float inv=1.0f/(superSample*superSample);
		for(int y=0, i=0; y<height; y+=superSample) 
		{
			for(int x=0; x<width; x+=superSample, i+=3) 
			{
				float X=0.0f;
				float Y=0.0f;
				float Z=0.0f;
				for(int sy=0; sy<superSample; sy++)
				{
					for(int sx=0; sx<superSample; sx++)
					{
						int offset=3*((x+sx+(y+sy)*width));
						X+=xyz[offset+0];
						Y+=xyz[offset+1];
						Z+=xyz[offset+2];
					}
				}
				rescaled[i+0]=X*inv;
				rescaled[i+1]=Y*inv;
				rescaled[i+2]=Z*inv;
			}
		}
		return new BitmapXYZ(width/superSample,height/superSample,rescaled);
	}
	else
		return new BitmapXYZ(width,height,xyz);
}

int IGIBitmapReader::read32i(CFile&s)
{
	int i;
	byte data;
	s.Read(&data,sizeof(data));
	i=data;
	s.Read(&data,sizeof(data));
	i|=data<<8;
	s.Read(&data,sizeof(data));
	i|=data<<16;
	s.Read(&data,sizeof(data));
	i|=data<<24;

	return i;
}

float IGIBitmapReader::read32f(CFile&s) 
{
	return MathUtil::intBitsToFloat(read32i(s));
}