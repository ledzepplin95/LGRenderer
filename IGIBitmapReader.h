#pragma once
#include "bitmapreader.h"

class IGIBitmapReader :
	public BitmapReader
{
	LG_DECLARE_DYNCREATE(IGIBitmapReader);

public:
    Bitmap* load(CString fileName,BOOL isLinear)const;

private:
	static int read32i(CFile&s);
    static float read32f(CFile&s);	
};
