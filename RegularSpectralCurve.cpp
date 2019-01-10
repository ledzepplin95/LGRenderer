#include "StdAfx.h"

#include "RegularSpectralCurve.h"
#include "float.h"

LG_IMPLEMENT_DYNCREATE(RegularSpectralCurve,SpectralCurve)

RegularSpectralCurve::RegularSpectralCurve(float spec[],float lMin,float lMax)
{
    set(spec,lMin,lMax);
}

void RegularSpectralCurve::set(float spec[],float lMin,float lMax)
{
	lambdaMin=lMin;
	lambdaMax=lMax;

	int index=0;
	float aa=spec[index];
	while(aa<FLT_MAX-1)
	{    
		spectrum.push_back(aa);
		index++;
		aa=spec[index];
	}

	delta=(lambdaMax-lambdaMin)/(spectrum.size()-1);
	invDelta=1.0f/delta;
}


float RegularSpectralCurve::sample(float lambda)const
{
	if(lambda<lambdaMin || lambda>lambdaMax)
		return 0.0f;

	float x=(lambda-lambdaMin)*invDelta;
	int b0=(int)x;
	int b1=min(b0+1,spectrum.size()-1);
	float dx=x-b0;

	return (1.0f-dx)*spectrum[b0]+dx*spectrum[b1];
}

RegularSpectralCurve& RegularSpectralCurve::operator=(const RegularSpectralCurve&rsc)
{
	if(this==&rsc) return *this;
    
    spectrum=rsc.spectrum;
	lambdaMin=rsc.lambdaMin;
	lambdaMax=rsc.lambdaMax;
	delta=rsc.delta;
	invDelta=rsc.invDelta;

	return *this;
}