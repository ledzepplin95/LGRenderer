#pragma once

class ColorFactory
{
	static CString cs1;
	static CString cs2;
	static CString cs3;
	static CString cs4;
	static CString cs5;

public:
    static CString getInternalColorSpace();
    static int getRequiredDataValues(CString colorSpace);
	static Color createColor(CString colorSpace,float data[3]);	
	static Color createColor(CString colorSpace,const vector<float>&data);
};
