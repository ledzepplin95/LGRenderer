#include "StdAfx.h"
#include "ConstantSpectralCurve.h"

LG_IMPLEMENT_DYNCREATE(ConstantSpectralCurve,SpectralCurve)

ConstantSpectralCurve::ConstantSpectralCurve(float a)
{
	amp=a;
}

void ConstantSpectralCurve::set(float a)
{
    amp=a;
}

float ConstantSpectralCurve::sample(float lambda) const
{
	return amp;
}

ConstantSpectralCurve& ConstantSpectralCurve::operator=(const ConstantSpectralCurve&csc)
{
	if(this==&csc) return *this;

    amp=csc.amp;

	return *this;
}
