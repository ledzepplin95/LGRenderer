#pragma once

#include "bitmapreader.h"

class HDRBitmapReader :
	public BitmapReader
{
	LG_DECLARE_DYNCREATE(HDRBitmapReader);

public:
	Bitmap* load(CString fileName,BOOL isLinear)const;

private:
	void readFlatRGBE(int rasterPos,int numPixels,CFile&f,vector<int>&pixels)const;
};
