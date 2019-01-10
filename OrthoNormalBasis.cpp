#include "StdAfx.h"

#include "OrthoNormalBasis.h"
#include "math.h"

OrthoNormalBasis::~OrthoNormalBasis(void)
{
}

OrthoNormalBasis::OrthoNormalBasis() 
{
	u.set(0,0,0);
	v.set(0,0,0);
	w.set(0,0,0);
}

void OrthoNormalBasis::flipU()
{
	u.negate();
}

void OrthoNormalBasis::flipV()
{
	v.negate();
}

void OrthoNormalBasis::flipW() 
{
	w.negate();
}

void OrthoNormalBasis::swapUV() 
{
	Vector3 t=u;
	u=v;
	v=t;
}

void OrthoNormalBasis::swapVW() 
{
	Vector3 t=v;
	v=w;
	w=t;
}

void OrthoNormalBasis::swapWU()
{
	Vector3 t=w;
	w=u;
	u=t;
}

Vector3 OrthoNormalBasis::transform(const Vector3&a,Vector3&dest)const
{
	dest.x=(a.x*u.x)+(a.y*v.x)+(a.z*w.x);
	dest.y=(a.x*u.y)+(a.y*v.y)+(a.z*w.y);
	dest.z=(a.x*u.z)+(a.y*v.z)+(a.z*w.z);

	return dest;
}

Vector3 OrthoNormalBasis::transform(Vector3&a) const
{
	float x=(a.x*u.x)+(a.y*v.x)+(a.z*w.x);
	float y=(a.x*u.y)+(a.y*v.y)+(a.z*w.y);
	float z=(a.x*u.z)+(a.y*v.z)+(a.z*w.z);

	return a.set(x,y,z);
}

Vector3 OrthoNormalBasis::untransform(const Vector3&a,Vector3&dest) const
{
	dest.x=Vector3::dot(a,u);
	dest.y=Vector3::dot(a,v);
	dest.z=Vector3::dot(a,w);

	return dest;
}

Vector3 OrthoNormalBasis::untransform(Vector3&a) const
{
	float x=Vector3::dot(a,u);
	float y=Vector3::dot(a,v);
	float z=Vector3::dot(a,w);

	return a.set(x,y,z);
}

float OrthoNormalBasis::untransformX(const Vector3&a) const
{
	return Vector3::dot(a,u);
}

float OrthoNormalBasis::untransformY(const Vector3&a) const
{
	return Vector3::dot(a,v);
}

float OrthoNormalBasis::untransformZ(const Vector3&a) const 
{
	return Vector3::dot(a,w);
}

OrthoNormalBasis OrthoNormalBasis::makeFromW(const Vector3&vw) 
{
	OrthoNormalBasis onb;
	Vector3 w=vw;
	w.normalize(onb.w);
	if( (fabs(onb.w.x)<fabs(onb.w.y)) 
		&& (fabs(onb.w.x)<fabs(onb.w.z)) ) 
	{
		onb.v.x=0.0f;
		onb.v.y=onb.w.z;
		onb.v.z=-onb.w.y;
	}
	else if( fabs(onb.w.y)<fabs(onb.w.z) )
	{
		onb.v.x=onb.w.z;
		onb.v.y=0.0f;
		onb.v.z=-onb.w.x;
	} 
	else
	{
		onb.v.x=onb.w.y;
		onb.v.y=-onb.w.x;
		onb.v.z=0.0f;
	}
	Vector3::cross(onb.v.normalize(),onb.w,onb.u);

	return onb;
}

OrthoNormalBasis OrthoNormalBasis::makeFromWV(const Vector3&w,const Vector3&v)
{
	OrthoNormalBasis onb;
	w.normalize(onb.w);
	Vector3::cross(v,onb.w,onb.u).normalize();
	Vector3::cross(onb.w,onb.u,onb.v);

	return onb;
}

OrthoNormalBasis& OrthoNormalBasis::operator =(const OrthoNormalBasis &onb)
{
	if(this==&onb) return *this;

	u=onb.u;
	v=onb.v;
	w=onb.w;

	return *this;
}

BOOL OrthoNormalBasis::operator ==(const OrthoNormalBasis &onb) const
{
	if(u!=onb.u) return FALSE;
	if(v!=onb.v) return FALSE;
	if(w!=onb.w) return FALSE;

	return TRUE;
}

BOOL OrthoNormalBasis::operator !=(const OrthoNormalBasis &onb) const
{
	if(u!=onb.u) return TRUE;
	if(v!=onb.v) return TRUE;
	if(w!=onb.w) return TRUE;

	return FALSE;
}
