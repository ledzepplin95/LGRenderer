#include "StdAfx.h"

#include "Ray.h"
#include <float.h>
#include "vector3.h"
#include "point3.h"
#include "Matrix4.h"

float Ray::EPSILON=0.0f;

Ray::Ray(void)
{
	ox=oy=oz=0.0f;
	dx=dy=dz=0.0f;
	tMin=0.0f;
    tMax=-1.0f;
}

Ray::Ray(float oox,float ooy,float ooz,float ddx,float ddy,float ddz) 
{
	ox=oox;
	oy=ooy;
	oz=ooz;
	dx=ddx;
	dy=ddy;
	dz=ddz;
	float inside=1.0f/(float)sqrt(ddx*ddx+ddy*ddy+ddz*ddz);
	dx*=inside;
	dy*=inside;
	dz*=inside;
	tMin=EPSILON;
	tMax=FLT_MAX;
}

Ray::Ray(const Point3&o,const Vector3&d) 
{
	ox=o.x;
	oy=o.y;
	oz=o.z;
	dx=d.x;
	dy=d.y;
	dz=d.z;
	float inside=1.0f/sqrt(dx*dx+dy*dy+dz*dz);
	dx*=inside;
	dy*=inside;
	dz*=inside;
	tMin=EPSILON;
	tMax=FLT_MAX;
}

Ray::Ray(const Point3&a,const Point3&b) 
{
	ox=a.x;
	oy=a.y;
	oz=a.z;
	dx=b.x-ox;
	dy=b.y-oy;
	dz=b.z-oz;
	tMin=EPSILON;
	float dist=(float)sqrt(dx*dx+dy*dy+dz*dz);
	float inside=1.0f/dist;
	dx*=inside;
	dy*=inside;
	dz*=inside;
	tMax=dist-EPSILON;
}

Ray Ray::transform(const Matrix4&m) const  
{
	if(m==Matrix4()) return *this;
	Ray r;
	r.ox=m.transformPX(ox,oy,oz);
	r.oy=m.transformPY(ox,oy,oz);
	r.oz=m.transformPZ(ox,oy,oz);
	r.dx=m.transformVX(dx,dy,dz);
	r.dy=m.transformVY(dx,dy,dz);
	r.dz=m.transformVZ(dx,dy,dz);
	r.tMin=tMin;
	r.tMax=tMax;

	return r;
}

void Ray::normalize() 
{
	float inside=1.0f/(float)sqrt(dx*dx+dy*dy+dz*dz);
	dx*=inside;
	dy*=inside;
	dz*=inside;
}

float Ray::getMin() const
{
	return tMin;
}

float Ray::getMax() const
{
	return tMax;
}

Vector3 Ray::getDirection() const
{
	return Vector3(dx,dy,dz).normalize();
}

BOOL Ray::isInside(float t) const
{
	return (tMin<=t) && (t<=tMax);
}

Point3 Ray::getPoint(Point3&dest) const
{
	dest.x=ox+(tMax*dx);
	dest.y=oy+(tMax*dy);
	dest.z=oz+(tMax*dz);

	return dest;
}

float Ray::dot(const Vector3&v)const 
{
	return dx*v.x+dy*v.y+dz*v.z;
}

float Ray::dot(float vx,float vy,float vz) const
{
	return dx*vx+dy*vy+dz*vz;
}

void Ray::setMax(float t) 
{
	tMax=t;
}

void Ray::reverseDirection()
{
	dx*=-1.0;
	dy*=-1.0;
	dz*=-1.0;
}

BOOL Ray::isNull()const
{
	return tMax<0.0f;
}

Ray& Ray::operator=(const Ray&r)
{
	if(this==&r) return *this;

    ox=r.ox;
	oy=r.oy;
	oz=r.oz;
	dx=r.dx;
	dy=r.dy;
	dz=r.dz;
	tMin=r.tMin;
	tMax=r.tMax;

	return *this;
}

BOOL Ray::operator!=(const Ray&r) const
{
	const float tol=1.0e-6f;
	if(fabs(ox-r.ox)>tol) return TRUE;
	if(fabs(oy-r.oy)>tol) return TRUE;
	if(fabs(oz-r.oz)>tol) return TRUE;
	if(fabs(dx-r.dx)>tol) return TRUE;
    if(fabs(dy-r.dy)>tol) return TRUE;
	if(fabs(dz-r.dz)>tol) return TRUE;
	if(fabs(tMin-r.tMin)>tol) return TRUE;
	if(fabs(tMax-r.tMax)>tol) return TRUE;

	return FALSE;
}

BOOL Ray::operator==(const Ray&r) const
{
	const float tol=1.0e-6f;
	if(fabs(ox-r.ox)>tol) return FALSE;
	if(fabs(oy-r.oy)>tol) return FALSE;
	if(fabs(oz-r.oz)>tol) return FALSE;
	if(fabs(dx-r.dx)>tol) return FALSE;
	if(fabs(dy-r.dy)>tol) return FALSE;
	if(fabs(dz-r.dz)>tol) return FALSE;
	if(fabs(tMin-r.tMin)>tol) return FALSE;
	if(fabs(tMax-r.tMax)>tol) return FALSE;

	return TRUE;
}


