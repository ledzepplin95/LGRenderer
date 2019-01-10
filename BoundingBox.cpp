#include "StdAfx.h"

#include "BoundingBox.h"
#include <float.h>
#include "MathUtil.h"
#include "vector3.h"

BoundingBox::~BoundingBox(void)
{
}

BoundingBox::BoundingBox() 
{
	minimum.set(FLT_MAX,FLT_MAX,FLT_MAX);
	maximum.set(-FLT_MAX,-FLT_MAX,-FLT_MAX);
}

BoundingBox::BoundingBox(const BoundingBox&b) 
{
	minimum.set(b.minimum);
	maximum.set(b.maximum);
}

BoundingBox::BoundingBox(const Point3&p) 
{
	minimum.set(p.x,p.y,p.z);
	maximum.set(p.x,p.y,p.z);	
}

BoundingBox::BoundingBox(float x,float y,float z) 
{
	minimum.set(x,y,z);
	maximum.set(x,y,z);
}

BoundingBox::BoundingBox(float size) 
{
	minimum.set(-size,-size,-size);
	maximum.set(size,size,size);
}

void BoundingBox::set(const BoundingBox&b)
{
	minimum.set(b.minimum);
	maximum.set(b.maximum);
}

void BoundingBox::set(const Point3&p)
{
    set(p.x,p.y,p.z);
}

void BoundingBox::set(float x,float y,float z)
{
	minimum.set(x,y,z);
	maximum.set(x,y,z);
}

void BoundingBox::set(float size)
{
	minimum.set(-size,-size,-size);
	maximum.set(size,size,size);
}

Point3 BoundingBox::getMinimum() const 
{
	return minimum;
}

Point3 BoundingBox::getMaximum() const
{
	return maximum;
}

Point3 BoundingBox::getCenter() const
{
	return Point3::mid(minimum,maximum,Point3());
}

Point3 BoundingBox::getCorner(int i) const
{
	float x=(i&1)==0?minimum.x:maximum.x;
	float y=(i&2)==0?minimum.y:maximum.y;
	float z=(i&4)==0?minimum.z:maximum.z;

	return Point3(x,y,z);
}

float BoundingBox::getBound(int i) const
{
	switch(i) 
	{
	case 0:
		return minimum.x;
	case 1:
		return maximum.x;
	case 2:
		return minimum.y;
	case 3:
		return maximum.y;
	case 4:
		return minimum.z;
	case 5:
		return maximum.z;
	default:
		return 0;
	}
}

Vector3 BoundingBox::getExtents() const
{	
	return Point3::sub(maximum,minimum,Vector3());
}

float BoundingBox::getArea() const
{
	Vector3 w=getExtents();
	float ax=max(w.x,0.0f);
	float ay=max(w.y,0.0f);
	float az=max(w.z,0.0f);

	return 2.0*(ax*ay+ay*az+az*ax);
}

float BoundingBox::getVolume() const
{
	Vector3 w=getExtents();
	float ax=max(w.x,0.0);
	float ay=max(w.y,0.0);
	float az=max(w.z,0.0);

	return ax*ay*az;
}

void BoundingBox::enlargeUlps() 
{
	float eps=0.0001f;
	minimum.x-=max(eps,MathUtil::ulp(minimum.x));
	minimum.y-=max(eps,MathUtil::ulp(minimum.y));
	minimum.z-=max(eps,MathUtil::ulp(minimum.z));
	maximum.x+=max(eps,MathUtil::ulp(maximum.x));
	maximum.y+=max(eps,MathUtil::ulp(maximum.y));
	maximum.z+=max(eps,MathUtil::ulp(maximum.z));
}

BOOL BoundingBox::isEmpty() const
{
	return (maximum.x<minimum.x) || (maximum.y<minimum.y)
		|| (maximum.z<minimum.z);
}

BOOL BoundingBox::intersects(const BoundingBox&b) const
{
	return ( (minimum.x<=b.maximum.x) 
		&& (maximum.x>=b.minimum.x) && (minimum.y<=b.maximum.y) 
		&& (maximum.y>=b.minimum.y) && (minimum.z<=b.maximum.z)
		&& (maximum.z>=b.minimum.z) );
}

BOOL BoundingBox::contains(const Point3&p) const
{
	return ( (p.x>=minimum.x) && (p.x<=maximum.x)
		&& (p.y>=minimum.y) && (p.y<=maximum.y) 
		&& (p.z>=minimum.z) && (p.z<=maximum.z));
}

BOOL BoundingBox::contains(float x,float y,float z) const 
{
	return ( (x>=minimum.x) && (x<=maximum.x) 
		&&(y>=minimum.y) && (y<=maximum.y) 
		&& (z>=minimum.z) && (z<=maximum.z));
}

void BoundingBox::include(const Point3&p) 
{	
	if(p.x<minimum.x)
		minimum.x=p.x;
	if(p.x>maximum.x)
		maximum.x=p.x;
	if(p.y<minimum.y)
		minimum.y=p.y;
	if(p.y>maximum.y)
		maximum.y=p.y;
	if(p.z<minimum.z)
		minimum.z=p.z;
	if(p.z>maximum.z)
		maximum.z=p.z;	
}

void BoundingBox::include(float x,float y,float z)
{
	if(x<minimum.x)
		minimum.x=x;
	if(x>maximum.x)
		maximum.x=x;
	if(y<minimum.y)
		minimum.y=y;
	if(y>maximum.y)
		maximum.y=y;
	if(z<minimum.z)
		minimum.z=z;
	if(z>maximum.z)
		maximum.z=z;
}

void BoundingBox::include(const BoundingBox&b)
{	
	if(b.minimum.x<minimum.x)
		minimum.x=b.minimum.x;
	if(b.maximum.x>maximum.x)
		maximum.x=b.maximum.x;
	if(b.minimum.y<minimum.y)
		minimum.y=b.minimum.y;
	if(b.maximum.y>maximum.y)
		maximum.y=b.maximum.y;
	if(b.minimum.z<minimum.z)
		minimum.z=b.minimum.z;
	if(b.maximum.z>maximum.z)
		maximum.z=b.maximum.z;	
}

CString BoundingBox::toString()const 
{
	CString str;
	str.Format(_T("(%.2f, %.2f, %.2f) жа (%.2f, %.2f, %.2f)"), 
		minimum.x,minimum.y,minimum.z,maximum.x,maximum.y,maximum.z);

	return str;
}

BoundingBox& BoundingBox::operator=(const BoundingBox&b)
{
	if(this==&b) return *this;

	minimum=b.minimum;
	maximum=b.maximum;

	return *this;
}

BOOL BoundingBox::operator!=(const BoundingBox&b)const 
{
    if(minimum!=b.minimum) return TRUE;
	if(maximum!=b.maximum) return TRUE;

	return FALSE;
}

BOOL BoundingBox::operator==(const BoundingBox&b)const
{
	if(minimum!=b.minimum) return FALSE;
	if(maximum!=b.maximum) return FALSE;

	return TRUE;
}

Point3& BoundingBox::Maximum()
{
	return maximum;
}

Point3& BoundingBox::Minimum()
{
	return minimum;
}