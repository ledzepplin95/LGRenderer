#include "StdAfx.h"
#include "GaussianFilter.h"

LG_IMPLEMENT_DYNCREATE(GaussianFilter,Filter)

GaussianFilter::GaussianFilter() 
{
	es2=(float)-exp(-getSize()*getSize());
}

float GaussianFilter::getSize() const
{
	return 3.0f;
}

float GaussianFilter::get(float x,float y) const
{
	float gx=(float)exp(-x*x)+es2;
	float gy=(float)exp(-y*y)+es2;

	return gx*gy;
}