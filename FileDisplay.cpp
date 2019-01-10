#include "StdAfx.h"

#include "FileDisplay.h"
#include "fileutil.h"
#include "PluginRegistry.h"
#include "strutil.h"
#include "Color.h"

FileDisplay::~FileDisplay()
{
}

FileDisplay::FileDisplay(BOOL saveImage) 
{	
	fileName=saveImage?"output.png":"none.png";
	CString extension=StrUtil::trimExtensionLeft(
		FileUtil::getExtension(fileName).MakeLower());

	writer=PluginRegistry::bitmapWriterPlugins.createObject(extension,2);
}

FileDisplay::FileDisplay(CString fName)
{
	fileName=fName.IsEmpty()?"output.png":fName;
	CString extension=StrUtil::trimExtensionLeft(
		FileUtil::getExtension(fileName).MakeLower());

	writer=PluginRegistry::bitmapWriterPlugins.createObject(extension,2);
}

void FileDisplay::imageBegin(int w,int h,int bucketSize) 
{
	if(writer==NULL) return;
	try 
	{
		writer->openFile(fileName);
		writer->writeHeader(w,h,bucketSize);
	} 
	catch(...) 
	{
		StrUtil::PrintPrompt("准备显示图像时发生I/O错误");
	}
}

void FileDisplay::imagePrepare(int x,int y,int w,int h,int id) 
{
}

void FileDisplay::imageUpdate(int x,int y,int w,int h,
							  vector<Color>&data,vector<float>&alpha) 
{
	if(writer==NULL) return;
	try 
	{
		writer->writeTile(x,y,w,h,data,alpha);
	} 
	catch(...) 
	{		
	}
}

void FileDisplay::imageFill(int x,int y,int w,int h,
							Color&c,float alpha) 
{
	if(writer==NULL) return;

	vector<Color> colorTile;
	colorTile.resize(w*h);
	vector<float> alphaTile;
	alphaTile.resize(w*h);
	for(int i=0; i<colorTile.size(); i++) 
	{
		colorTile[i]=c;
		alphaTile[i]=alpha;
	}
	imageUpdate(x,y,w,h,colorTile,alphaTile);
}

void FileDisplay::imageEnd() 
{
	if(writer==NULL) return;
	try 
	{
		writer->closeFile();
	} 
	catch(...) 
	{		
	}
}