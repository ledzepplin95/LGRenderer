#include "StdAfx.h"

#include "IrregularSpectralCurve.h"
#include "strutil.h"
#include "float.h"

LG_IMPLEMENT_DYNCREATE(IrregularSpectralCurve,SpectralCurve)

IrregularSpectralCurve::IrregularSpectralCurve(float wls[],float ampls[]) 
{
    set(wls,ampls);
}

void IrregularSpectralCurve::set(float wls[],float ampls[])
{
	int index=0;
	float aa=wls[index];
	while(aa<FLT_MAX-1)
	{    
		wavelengths.push_back(aa);
		index++;
		aa=wls[index];
	}

	index=0;
	aa=ampls[index];
	while(aa<FLT_MAX-1)
	{    
		amplitudes.push_back(aa);
		index++;
		aa=ampls[index];
	}

	if(wavelengths.size()!=amplitudes.size())
		StrUtil::PrintPrompt("创建不规则谱曲线错误:%d波长和%d振幅", 
		wavelengths.size(),amplitudes.size());
	for(int i=1; i<wavelengths.size(); i++)
		if(wavelengths[i-1]>=wavelengths[i])
			StrUtil::PrintPrompt("创建不规则谱曲线错误: 值没有排序，在%d索引处错误",i);
}

float IrregularSpectralCurve::sample(float lambda)const
{
	if(wavelengths.size()==0)
		return 0;
	if(wavelengths.size()==1 || lambda<=wavelengths[0])
		return amplitudes[0];
	if(lambda>=wavelengths[wavelengths.size()-1])
		return amplitudes[wavelengths.size()-1];

	for(int i=1; i<wavelengths.size(); i++)
	{
		if(lambda<wavelengths[i]) 
		{
			float dx=(lambda-wavelengths[i-1]) 
				/(wavelengths[i]-wavelengths[i-1]);

			return (1.0f-dx)*amplitudes[i-1]+dx*amplitudes[i];
		}
	}

	return amplitudes[wavelengths.size()-1];
}

IrregularSpectralCurve& IrregularSpectralCurve::operator=(const IrregularSpectralCurve&irc)
{
	if(this==&irc) return *this;

    wavelengths=irc.wavelengths;
	amplitudes=irc.amplitudes;

	return *this;
}


