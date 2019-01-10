#include "StdAfx.h"

#include "ColorFactory.h"
#include "RGBSpace.h"
#include "BlackbodySpectrum.h"
#include "RegularSpectralCurve.h"
#include "StrUtil.h"
#include "xyzcolor.h"
#include "Color.h"

CString ColorFactory::cs1="sRGB nonlinear";
CString ColorFactory::cs2="sRGB linear";
CString ColorFactory::cs3="XYZ";
CString ColorFactory::cs4="blackbody";
CString ColorFactory::cs5="spectrum";

CString ColorFactory::getInternalColorSpace()
{
	return cs2;
}

int ColorFactory::getRequiredDataValues(CString colorSpace) 
{
	if(colorSpace.IsEmpty())
		return 3;
	if(colorSpace==cs1)
		return 3;
	else if(colorSpace==cs2)
		return 3;
	else if(colorSpace==cs3)
		return 3;
	else if(colorSpace==cs4)
		return 1;
	else if(colorSpace.Find(cs5)==0)
		return -1;
	else
		return -2;
}

Color ColorFactory::createColor(CString colorSpace,const vector<float>&data) 
{
	float d[3];
	memcpy(d,&data[0],sizeof(float)*3);

	return createColor(colorSpace,d);
}

Color ColorFactory::createColor(CString colorSpace,float data[3]) 
{
	int required=getRequiredDataValues(colorSpace);
	if(required==-2)
	{
		StrUtil::PrintPrompt("未知的颜色空间%s",colorSpace);

		return Color();
	}
	if( required!=-1 && required!=3 )
	{
		StrUtil::PrintPrompt("%d  %d",required,3);

		return Color();
	}

	if(colorSpace.IsEmpty())
		return Color(data[0],data[1],data[2]);
	else if(colorSpace==cs1)
		return Color(data[0],data[1],data[2]).toLinear();
	else if(colorSpace==cs2)
		return Color(data[0],data[1],data[2]);
	else if(colorSpace==cs3)
	{
		Color c;
		RGBSpace::SRGB.convertXYZtoRGB(XYZColor(data[0],data[1],data[2]),c);

        return c;
	}		
	else if(colorSpace==cs4)
	{
		Color c;
		RGBSpace::SRGB.convertXYZtoRGB(BlackbodySpectrum(data[0]).toXYZ(),c);

		return c;
	}
	else if(colorSpace.Find(cs5)==0)
	{
		vector<CString> tokens=StrUtil::split(colorSpace);
		if(tokens.size()!=3)
		{
			StrUtil::PrintPrompt("无效的谱编码");
            return Color();
		}
		float lambdaMin=atof(tokens[1]);
		float lambdaMax=atof(tokens[2]);
        Color c;
	    RGBSpace::SRGB.convertXYZtoRGB(RegularSpectralCurve(data,
			lambdaMin,lambdaMax).toXYZ(),c);

		return c;
	}

	return Color();
}

