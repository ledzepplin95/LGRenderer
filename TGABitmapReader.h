#pragma once
#include "bitmapreader.h"

class TGABitmapReader :
	public BitmapReader
{
	LG_DECLARE_DYNCREATE(TGABitmapReader);

public:
	Bitmap* load(CString fileName,BOOL isLinear)const;

private:
	static int CHANNEL_INDEX[];	
};
