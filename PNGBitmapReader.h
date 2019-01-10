#pragma once
#include "bitmapreader.h"

class PNGBitmapReader :
	public BitmapReader
{
	LG_DECLARE_DYNCREATE(PNGBitmapReader);

public:
	Bitmap* load(CString fileName,BOOL isLinear)const;	
};
