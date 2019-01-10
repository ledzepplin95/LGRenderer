#include "StdAfx.h"
#include "BlackbodySpectrum.h"

LG_IMPLEMENT_DYNCREATE(BlackbodySpectrum,SpectralCurve)

BlackbodySpectrum::BlackbodySpectrum(float t) 
{
	temp=t;
}

float BlackbodySpectrum::sample(float lambda) const
{
	double wavelength=lambda*1e-9;

	return (float)((3.74183e-16*pow(wavelength, 
		-5.0))/(exp(1.4388e-2/(wavelength*temp))-1.0));
}