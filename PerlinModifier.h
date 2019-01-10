#pragma once
#include "modifier.h"

class PerlinModifier :
	public Modifier
{
	LG_DECLARE_DYNCREATE(PerlinModifier);

public:
	PerlinModifier();

	BOOL update(ParameterList&pl,LGAPI&api);
	void modify(ShadingState&state)const;

private:
	int _function;
	float _scale;
	float _size;

	double f(double x,double y,double z)const;
	static double stripes(double x,double f);
	static double turbulence(double x,double y,double z,double freq);
	static double noise(double x,double y,double z,double freq);	
};
