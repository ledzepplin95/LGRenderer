#pragma once
#include "spectralcurve.h"
#include "RegularSpectralCurve.h"

class ChromaticitySpectrum :
	public SpectralCurve
{
	LG_DECLARE_DYNCREATE(ChromaticitySpectrum);
public:
	ChromaticitySpectrum(){}
	ChromaticitySpectrum(float x,float y);

	float sample(float lambda)const;
	static XYZColor get(float x,float y);

private:
    static float S0Amplitudes[];
    static float S1Amplitudes[];
	static float S2Amplitudes[];

	static RegularSpectralCurve kS0Spectrum;
	static RegularSpectralCurve kS1Spectrum;
	static RegularSpectralCurve kS2Spectrum;

	static XYZColor S0xyz;
	static XYZColor S1xyz;
	static XYZColor S2xyz;

	float M1,M2;
};
