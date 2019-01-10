#include "StdAfx.h"
#include "XYZColor.h"

XYZColor::XYZColor(void)
{
	X=Y=Z=0.0f;
}

XYZColor::~XYZColor(void)
{
}

XYZColor::XYZColor(float XX,float YY,float ZZ) 
{
	X=XX;
	Y=YY;
	Z=ZZ;
}

void XYZColor::set(float XX,float YY,float ZZ)
{
	X=XX;
	Y=YY;
	Z=ZZ;
}

float XYZColor::getX() const
{
	return X;
}

float XYZColor::getY() const
{
	return Y;
}

float XYZColor::getZ() const
{
	return Z;
}

XYZColor& XYZColor::mul(float s) 
{
	X*=s;
	Y*=s;
	Z*=s;

	return *this;
}

void XYZColor::normalize() 
{
	float XYZ=X+Y+Z;
	if(XYZ<1e-6f)
		return;
	float s=1.0f/XYZ;
	X*=s;
	Y*=s;
	Z*=s;
}

CString XYZColor::toString()const
{
	CString str;
	str.Format(_T("(%.3f, %.3f, %.3f)"),X,Y,Z);

	return str;
}

XYZColor& XYZColor::operator=(const XYZColor&xc)
{
	if(this==&xc) return *this;
 
	X=xc.X;
	Y=xc.Y;
	Z=xc.Z;	

	return *this;
}