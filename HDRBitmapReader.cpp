#include "StdAfx.h"

#include "HDRBitmapReader.h"
#include "BitmapRGBE.h"
#include "strutil.h"

LG_IMPLEMENT_DYNCREATE(HDRBitmapReader,BitmapReader)

Bitmap* HDRBitmapReader::load(CString fileName,BOOL isLinear)const 
{		
	CFile f;
	if( !f.Open(fileName,CFile::modeRead | CFile::typeBinary) )
		return NULL;

	BOOL parseWidth=FALSE,parseHeight=FALSE;
	int width=0,height=0;
	int last=0;
	byte data;
	while(width==0 || height==0 || last!='\n') 
	{
		f.Read(&data,sizeof(data));
		int n=data;
		switch(n) 
		{
		case 'Y':
			parseHeight=last=='-';
			parseWidth=FALSE;
			break;
		case 'X':
			parseHeight=FALSE;
			parseWidth=last=='+';
			break;
		case ' ':
			parseWidth&=width==0;
			parseHeight&=height==0;
			break;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			if(parseHeight)
				height=10*height+(n-'0');
			else if(parseWidth)
				width=10*width+(n-'0');
			break;
		default:
			parseWidth=parseHeight=FALSE;
			break;
		}
		last=n;
	}	
	vector<int> pixels;
	pixels.resize(width*height);
	if( (width<8) || (width>0x7fff) )
	{		
		readFlatRGBE(0,width*height,f,pixels);
	} 
    else
	{
		int rasterPos=0;
		int numScanlines=height;
		vector<int> scanlineBuffer;
		scanlineBuffer.resize(4*width);
		while(numScanlines>0) 
	   {
		   f.Read(&data,sizeof(data));
			int r=data;
			f.Read(&data,sizeof(data));
			int g=data;
			f.Read(&data,sizeof(data));
			int b=data;
			f.Read(&data,sizeof(data));
			int e=data;
			if( (r!=2) || (g!=2) || ((b&0x80)!=0) )
	       {			
				pixels[rasterPos]=(r<<24) | (g<<16) | (b<<8) | e;
				readFlatRGBE(rasterPos+1,width*numScanlines-1,f,pixels);
				break;
			}

			if( ((b<<8)|e)!=width )
			{
				StrUtil::PrintPrompt("无效扫描线宽度");
				return NULL;
			}
			int p=0;			
			for(int i=0; i<4; i++) 
	       {
			   if(p%width!=0)
			   {
				   StrUtil::PrintPrompt("存取扫描线数据未对齐");
				   return NULL;
			   }
			   int end=(i+1)*width;
			   while(p<end)
			   {
                   f.Read(&data,sizeof(data));
				   int b0=data;
				   f.Read(&data,sizeof(data));
				   int b1=data;
				   if(b0>128) 
				   {					
					   int count=b0-128;
					   if( (count==0) || (count>(end-p)) )
					   {
						   StrUtil::PrintPrompt("错误的扫描线数据，无效的RLE行程");
						   return NULL;
					   }

					   while(count-->0) 
					   {
						   scanlineBuffer[p]=b1;
						   p++;
					   }
				   } 
				   else 
				   {						
					   int count=b0;
					   if( (count==0) || (count>(end-p)) )
					   {
						   StrUtil::PrintPrompt("错误的扫描线数据，无效的数量");
						   return NULL;
					   }
					   scanlineBuffer[p]=b1;
					   p++;
					   if(--count>0) 
					   {
						   for(int x=0; x<count; x++)
						   {
							   f.Read(&data,sizeof(data));
							   scanlineBuffer[p+x]=data;
						   }
						   p+=count;
					   }
				   }
			   }
			}			
			for(int i=0; i<width; i++) 
			{     
				r=scanlineBuffer[i];
				g=scanlineBuffer[i+width];
				b=scanlineBuffer[i+2*width];
				e=scanlineBuffer[i+3*width];
				pixels[rasterPos]=(r<<24) | (g<<16) | (b<<8) | e;
				rasterPos++;
			}
			numScanlines--;
		}
	}
	f.Close();	

	for(int y=0, i=0, ir=(height-1)*width; y<height/2; y++, ir-=width) 
	{
		for(int x=0, i2=ir; x<width; x++, i++, i2++) 
	    {
			int t=pixels[i];
			pixels[i]=pixels[i2];
			pixels[i2]=t;
		}
	}

	return new BitmapRGBE(width,height,pixels);
}

void HDRBitmapReader::readFlatRGBE(int rasterPos,int numPixels,CFile&f,vector<int>&pixels)const 
{
	byte data;
	while(numPixels-->0) 
	{
		f.Read(&data,sizeof(data));
		int r=data;
		f.Read(&data,sizeof(data));
		int g=data;
		f.Read(&data,sizeof(data));
		int b=data;
		f.Read(&data,sizeof(data));
		int e=data;
		pixels[rasterPos]=(r<<24) | (g<<16) | (b<<8) | e;
		rasterPos++;
	}
}