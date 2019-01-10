#include "StdAfx.h"

#include "Point3.h"
#include "math.h"
#include "Vector3.h"

Point3::Point3(void)
{
	x=y=z=0.0f;
}

Point3::~Point3(void)
{
}

Point3::Point3(float xx,float yy,float zz) 
{
	x=xx;
	y=yy;
	z=zz;
}

Point3::Point3(const Point3&p) 
{
	x=p.x;
	y=p.y;
	z=p.z;
}

float Point3::get(int i) const
{
	switch(i) 
	{
		case 0:
			return x;
		case 1:
			return y;
		default:
			return z;
	}
}

float Point3::distanceTo(const Point3&p)const
{
	float dx=x-p.x;
	float dy=y-p.y;
	float dz=z-p.z;

	return (float)sqrt((dx*dx)+(dy*dy)+(dz*dz));
}

float Point3::distanceTo(float px,float py,float pz)const
{
	float dx=x-px;
	float dy=y-py;
	float dz=z-pz;

	return (float)sqrt((dx*dx)+(dy*dy)+(dz*dz));
}

float Point3::distanceToSquared(const Point3&p)const 
{
	float dx=x-p.x;
	float dy=y-p.y;
	float dz=z-p.z;

	return (dx*dx)+(dy*dy)+(dz*dz);
}

float Point3::distanceToSquared(float px,float py,float pz)const
{
	float dx=x-px;
	float dy=y-py;
	float dz=z-pz;

	return (dx*dx)+(dy*dy)+(dz*dz);
}

Point3& Point3::set(float xx,float yy,float zz)
{
	x=xx;
	y=yy;
	z=zz;

	return *this;
}

Point3& Point3::set(const Point3&p) 
{
	x=p.x;
	y=p.y;
	z=p.z;

	return *this;
}

Point3 Point3::add(const Point3&p,const Vector3&v,Point3&dest)
{
	dest.x=p.x+v.x;
	dest.y=p.y+v.y;
	dest.z=p.z+v.z;

	return dest;
}

Vector3 Point3::sub(const Point3&p1,const Point3&p2,Vector3&dest) 
{
	dest.x=p1.x-p2.x;
	dest.y=p1.y-p2.y;
	dest.z=p1.z-p2.z;

	return dest;
}

Point3 Point3::mid(const Point3&p1,const Point3&p2,Point3&dest)
{
	dest.x=0.5f*(p1.x+p2.x);
	dest.y=0.5f*(p1.y+p2.y);
	dest.z=0.5f*(p1.z+p2.z);

	return dest;
}

Point3 Point3::blend(const Point3&p0,const Point3&p1,float b,Point3&dest)
{
	dest.x=(1.0f-b)*p0.x+b*p1.x;
	dest.y=(1.0f-b)*p0.y+b*p1.y;
	dest.z=(1.0f-b)*p0.z+b*p1.z;

	return dest;
}

Vector3 Point3::normal(const Point3&p0,const Point3&p1,const Point3&p2) 
{
	float edge1x=p1.x-p0.x;
	float edge1y=p1.y-p0.y;
	float edge1z=p1.z-p0.z;
	float edge2x=p2.x-p0.x;
	float edge2y=p2.y-p0.y;
	float edge2z=p2.z-p0.z;
	float nx=edge1y*edge2z-edge1z*edge2y;
	float ny=edge1z*edge2x-edge1x*edge2z;
	float nz=edge1x*edge2y-edge1y*edge2x;

	return Vector3(nx,ny,nz);
}

Vector3 Point3::normal(const Point3&p0,const Point3&p1,const Point3&p2,Vector3&dest)
{
	float edge1x=p1.x-p0.x;
	float edge1y=p1.y-p0.y;
	float edge1z=p1.z-p0.z;
	float edge2x=p2.x-p0.x;
	float edge2y=p2.y-p0.y;
	float edge2z=p2.z-p0.z;
	dest.x=edge1y*edge2z-edge1z*edge2y;
	dest.y=edge1z*edge2x-edge1x*edge2z;
	dest.z=edge1x*edge2y-edge1y*edge2x;

	return dest;
}

CString Point3::toString()const 
{
	CString str;
	str.Format(_T("%.2f, %.2f, %.2f"),x,y,z);

	return str;
}

Point3& Point3::operator=(const Point3&p)
{
	if(this==&p) return *this;

	x=p.x;
	y=p.y;
	z=p.z;	

	return *this;
}

Vector3 Point3::operator-(const Point3&p)
{
	Vector3 v;
	v.x=this->x-p.x;
	v.y=this->y-p.y;
	v.z=this->z-p.z;

	return v;
}

BOOL Point3::operator!=(const Point3&p) const
{
	const float tol=1.0e-6f;
	if(fabs(x-p.x)>tol) return TRUE;
	if(fabs(y-p.y)>tol) return TRUE;
	if(fabs(z-p.z)>tol) return TRUE;

	return FALSE;
}

BOOL Point3::operator==(const Point3&p)const
{
    const float tol=1.0e-6f;
	if(fabs(x-p.x)>tol) return FALSE;
	if(fabs(y-p.y)>tol) return FALSE;
	if(fabs(z-p.z)>tol) return FALSE;

	return TRUE;
}