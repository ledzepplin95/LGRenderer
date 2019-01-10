#include "StdAfx.h"
#include "BoxFilter.h"

LG_IMPLEMENT_DYNCREATE(BoxFilter,Filter)

float BoxFilter::getSize() const
{
	return 1.0f;
}

float BoxFilter::get(float x,float y)const
{
	return 1.0f;
}