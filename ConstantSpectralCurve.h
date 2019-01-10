#pragma once
#include "spectralcurve.h"

class ConstantSpectralCurve :
	public SpectralCurve
{
	LG_DECLARE_DYNCREATE(ConstantSpectralCurve);
public:
	ConstantSpectralCurve(){}
    ConstantSpectralCurve(float a);

	void set(float a);
	float sample(float lambda)const;
	ConstantSpectralCurve& operator=(const ConstantSpectralCurve&csc);

private:
	float amp;	
};
