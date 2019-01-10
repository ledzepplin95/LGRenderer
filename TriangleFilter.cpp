#include "StdAfx.h"
#include "TriangleFilter.h"

LG_IMPLEMENT_DYNCREATE(TriangleFilter,Filter)

float TriangleFilter::getSize() const
{
	return 2.0f;
}

float TriangleFilter::get(float x,float y) const
{
	return (1.0f-fabs(x))*(1.0f-fabs(y));
}