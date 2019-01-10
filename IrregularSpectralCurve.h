#pragma once

#include "spectralcurve.h"

class IrregularSpectralCurve :
	public SpectralCurve
{
	LG_DECLARE_DYNCREATE(IrregularSpectralCurve);
public:
	IrregularSpectralCurve(){}
    IrregularSpectralCurve(float wls[],float ampls[]);

	void set(float wls[],float ampls[]);
	float sample(float lambda)const;
	IrregularSpectralCurve& operator=(const IrregularSpectralCurve&isc);

private:
    vector<float> wavelengths;
	vector<float> amplitudes;
};
