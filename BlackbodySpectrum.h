#pragma once
#include "spectralcurve.h"

class BlackbodySpectrum :
	public SpectralCurve
{
	LG_DECLARE_DYNCREATE(BlackbodySpectrum);
public:
	BlackbodySpectrum(){}
	BlackbodySpectrum(float t);

	float sample(float lambda)const;

private:
	float temp;	
};
