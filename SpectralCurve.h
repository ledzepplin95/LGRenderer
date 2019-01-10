#pragma once

#include "LGObject.h"

class XYZColor;
class SpectralCurve : public LGObject
{
	LG_DECLARE_DYNCREATE(SpectralCurve);
public:
	virtual float sample(float lambda)const;
	XYZColor toXYZ()const;

private:
	static int WAVELENGTH_MIN;
	static int WAVELENGTH_MAX;

	static double CIE_xbar[];
	static double CIE_ybar[];
	static double CIE_zbar[];
	static int WAVELENGTH_STEP;
};
