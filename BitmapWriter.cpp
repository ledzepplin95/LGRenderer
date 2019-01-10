#include "stdafx.h"
#include "BitmapWriter.h"

LG_IMPLEMENT_DYNCREATE(BitmapWriter,LGObject)

void BitmapWriter::configure(CString option,CString value)
{
}

void BitmapWriter::openFile(CString fileName)
{
}

void BitmapWriter::writeHeader(int width,int height,int tileSize)
{
}

void BitmapWriter::writeTile(int x,int y,int w,int h, 
			   const vector<Color>&color,const vector<float>&alpha)
{
}

void BitmapWriter::closeFile()
{
}