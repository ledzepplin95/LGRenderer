#pragma once
#include "spectralcurve.h"

class RegularSpectralCurve :
	public SpectralCurve
{
	 LG_DECLARE_DYNCREATE(RegularSpectralCurve);
public:
	RegularSpectralCurve(){}
     RegularSpectralCurve(float spec[],float lMin,float lMax);

     void set(float spec[],float lMin,float lMax);
	 float sample(float lambda)const;
     RegularSpectralCurve& operator=(const RegularSpectralCurve&rsc);

private:
	vector<float> spectrum;
	float lambdaMin,lambdaMax;
	float delta,invDelta;
};
