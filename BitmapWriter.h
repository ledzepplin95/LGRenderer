#pragma once

#include "lgobject.h"

class BitmapWriter : public LGObject
{
	LG_DECLARE_DYNCREATE(BitmapWriter);

public:
	virtual void configure(CString option,CString value);
    virtual void openFile(CString fileName);
	virtual void writeHeader(int width,int height,int tileSize);
	virtual void writeTile(int x,int y,int w,int h, 
		const vector<Color>&color,const vector<float>&alpha);
	virtual void closeFile();	
};
