#include "StdAfx.h"

#include "Texture.h"
#include "MathUtil.h"
#include "FileUtil.h"
#include "strutil.h"
#include "BitmapBlack.h"
#include "BitmapReader.h"
#include "OrthoNormalBasis.h"
#include "PluginRegistry.h"
#include "Color.h"

Texture::Texture()
{
	bitmap=NULL;
}

Texture::Texture(CString fName,BOOL isL) 
{
	fileName=fName;
	isLinear=isL;
	loaded=0;
	bitmap=NULL;
}

Texture::~Texture()
{
	if(!bitmap) delete bitmap;
	bitmap=NULL;
}

void Texture::load() 
{
	CCriticalSection cs;
	cs.Lock();
	if(loaded!=0) 
	{
		cs.Unlock();
		return;
	}

	CString extension=StrUtil::trimExtensionLeft(
		FileUtil::getExtension(fileName));	
	StrUtil::PrintPrompt("从\"%s\"文件读取纹理位图...",fileName);
	BitmapReader *reader=PluginRegistry::bitmapReaderPlugins.createObject(extension);
	if(reader!=NULL)
	{
		bitmap=reader->load(fileName,isLinear);
		if(bitmap->getWidth()==0 
			|| bitmap->getHeight()==0 )
			bitmap=NULL;		
	}
	if(bitmap==NULL) 
	{
		StrUtil::PrintPrompt("位图读入失败");
		bitmap=new BitmapBlack();
	} 
	else
		StrUtil::PrintPrompt("纹理位图读入完成: 发现%dx%d个像素 ",
		bitmap->getWidth(),bitmap->getHeight());

	loaded=1;
	cs.Unlock();
}

Bitmap* Texture::getBitmap()const 
{
	if(loaded==0) return NULL;
		
	return bitmap;
}

Color Texture::getPixel(float x,float y)const
{	
	x=MathUtil::frac(x);
	y=MathUtil::frac(y);
	float dx=x*(bitmap->getWidth()-1);
	float dy=y*(bitmap->getHeight()-1);
	int ix0=(int)dx;
	int iy0=(int)dy;
	int ix1=(ix0+1)%bitmap->getWidth();
	int iy1=(iy0+1)%bitmap->getHeight();
	float u=dx-ix0;
	float v=dy-iy0;
	u=u*u*(3.0f-(2.0f*u));
	v=v*v*(3.0f-(2.0f*v));
	float k00=(1.0f-u)*(1.0f-v);
	Color c00=bitmap->readColor(ix0,iy0);
	float k01=(1.0f-u)*v;
	Color c01=bitmap->readColor(ix0,iy1);
	float k10=u*(1.0f-v);
	Color c10=bitmap->readColor(ix1,iy0);
	float k11=u*v;
	Color c11=bitmap->readColor(ix1,iy1);
	Color c=Color::mul(k00,c00);
	c.madd(k01,c01);
	c.madd(k10,c10);
	c.madd(k11,c11);

	return c;
}

Vector3 Texture::getNormal(float x,float y,const OrthoNormalBasis&basis) const
{
	vector<float> rgb=getPixel(x,y).getRGB();
	return basis.transform(
		Vector3(2*rgb[0]-1,2*rgb[1]-1,2*rgb[2]-1)).normalize();
}

Vector3 Texture::getBump(float x,float y,const OrthoNormalBasis&basis,float scale) const
{	
	float dx=1.0f/bitmap->getWidth();
	float dy=1.0f/bitmap->getHeight();
	float b0=getPixel(x,y).getLuminance();
	float bx=getPixel(x+dx,y).getLuminance();
	float by=getPixel(x,y+dy).getLuminance();
	return basis.transform(
		Vector3(scale*(b0-bx),scale*(b0-by),1)).normalize();
}
