#include "StdAfx.h"
#include "Point2.h"

Point2::Point2(void)
{
	x=y=0.0f;
}

Point2::~Point2(void)
{	
}

Point2::Point2(float xx,float yy)
{
	x=xx;
	y=yy;
}

Point2::Point2(const Point2&p)
{
	x=p.x;
	y=p.y;
}

Point2& Point2::set(float xx,float yy) 
{
	x=xx;
	y=yy;

	return *this;
}

Point2& Point2::set(const Point2&p) 
{
	x=p.x;
	y=p.y;

	return *this;
}

CString Point2::toString()const 
{
	CString str;
	str.Format(_T("%.2f, %.2f"),x,y);

	return str;
}

Point2& Point2::operator=(const Point2&p)
{
	if(this==&p) return *this;

	x=p.x;
	y=p.y;

	return *this;
}

BOOL Point2::operator==(const Point2&p)const
{
	const float tol=1.0e-6f;
	if(fabs(x-p.x)>tol) return FALSE;
	if(fabs(y-p.y)>tol) return FALSE;

	return TRUE;
}

BOOL Point2::operator!=(const Point2&p)const
{
	const float tol=1.0e-6f;
	if(fabs(x-p.x)>tol) return TRUE;
	if(fabs(y-p.y)>tol) return TRUE;
	
	return FALSE;
}