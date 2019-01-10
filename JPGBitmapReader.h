#pragma once
#include "bitmapreader.h"

class JPGBitmapReader :
	public BitmapReader
{
	LG_DECLARE_DYNCREATE(JPGBitmapReader);

public:
    Bitmap* load(CString fileName,BOOL isLinear)const;
};
