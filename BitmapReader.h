#pragma once

#include "LGObject.h"

class Bitmap;
class BitmapReader : public LGObject
{
	 LG_DECLARE_DYNCREATE(BitmapReader);
public:
     virtual Bitmap* load(CString fileName,BOOL isLinear)const;
};
