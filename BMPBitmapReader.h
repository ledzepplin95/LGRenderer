#pragma once
#include "bitmapreader.h"

class BMPBitmapReader :
	public BitmapReader
{
	LG_DECLARE_DYNCREATE(BMPBitmapReader);

public:
	Bitmap* load(CString fileName,BOOL isLinear)const;
};
