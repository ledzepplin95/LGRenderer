#include "StdAfx.h"

#include "PerlinVector.h"
#include "PerlinScalar.h"
#include "vector3.h"
#include "point3.h"
#include "point2.h"

float PerlinVector::P1x=0.34f;
float PerlinVector::P1y=0.66f;
float PerlinVector::P1z=0.237f;
float PerlinVector::P2x=0.011f;
float PerlinVector::P2y=0.845f;
float PerlinVector::P2z=0.037f;
float PerlinVector::P3x=0.34f;
float PerlinVector::P3y=0.12f;
float PerlinVector::P3z=0.9f;

Vector3 PerlinVector::snoise(float x) 
{
	return Vector3(PerlinScalar::snoise(x+P1x), 
		PerlinScalar::snoise(x+P2x),
		PerlinScalar::snoise(x+P3x));
}

Vector3 PerlinVector::snoise(float x,float y) 
{
	return Vector3(PerlinScalar::snoise(x+P1x,y+P1y),
		PerlinScalar::snoise(x+P2x,y+P2y), 
		PerlinScalar::snoise(x+P3x,y+P3y));
}

Vector3 PerlinVector::snoise(float x,float y,float z)
{
	return Vector3(PerlinScalar::snoise(x+P1x,y+P1y,z+P1z),
		PerlinScalar::snoise(x+P2x,y+P2y,z+P2z), 
		PerlinScalar::snoise(x+P3x,y+P3y,z+P3z));
}

Vector3 PerlinVector::snoise(float x,float y,float z,float t) 
{
	return Vector3(PerlinScalar::snoise(x+P1x,y+P1y,z+P1z,t), 
		PerlinScalar::snoise(x+P2x,y+P2y,z+P2z,t), 
		PerlinScalar::snoise(x+P3x,y+P3y,z+P3z,t));
}

Vector3 PerlinVector::snoise(const Point2&p) 
{
	return snoise(p.x,p.y);
}

Vector3 PerlinVector::snoise(const Point3&p) 
{
	return snoise(p.x,p.y,p.z);
}

Vector3 PerlinVector::snoise(const Point3&p,float t) 
{
	return snoise(p.x,p.y,p.z,t);
}

Vector3 PerlinVector::noise(float x)
{
	return Vector3(PerlinScalar::noise(x+P1x), 
		PerlinScalar::noise(x+P2x),
		PerlinScalar::noise(x+P3x));
}

Vector3 PerlinVector::noise(float x,float y) 
{
	return Vector3(PerlinScalar::noise(x+P1x,y+P1y),
		PerlinScalar::noise(x+P2x,y+P2y), 
		PerlinScalar::noise(x+P3x,y+P3y));
}

Vector3 PerlinVector::noise(float x,float y,float z) 
{
	return Vector3(PerlinScalar::noise(x+P1x,y+P1y,z+P1z), 
		PerlinScalar::noise(x+P2x,y+P2y,z+P2z),
		PerlinScalar::noise(x+P3x,y+P3y,z+P3z));
}

Vector3 PerlinVector::noise(float x,float y,float z,float t) 
{
	return Vector3(PerlinScalar::noise(x+P1x,y+P1y,z+P1z,t), 
		PerlinScalar::noise(x+P2x,y+P2y,z+P2z,t), 
		PerlinScalar::noise(x+P3x,y+P3y,z+P3z,t));
}

Vector3 PerlinVector::noise(const Point2&p) 
{
	return noise(p.x,p.y);
}

Vector3 PerlinVector::noise(const Point3&p) 
{
	return noise(p.x,p.y,p.z);
}

Vector3 PerlinVector::noise(const Point3&p,float t)
{
	return noise(p.x,p.y,p.z,t);
}

Vector3 PerlinVector::pnoise(float x,float period) 
{
	return Vector3(PerlinScalar::pnoise(x+P1x,period),
		PerlinScalar::pnoise(x+P2x,period), 
		PerlinScalar::pnoise(x+P3x,period));
}

Vector3 PerlinVector::pnoise(float x,float y,float w,float h) 
{
	return Vector3(PerlinScalar::pnoise(x+P1x,y+P1y,w,h), 
		PerlinScalar::pnoise(x+P2x,y+P2y,w,h), 
		PerlinScalar::pnoise(x+P3x,y+P3y,w,h));
}

Vector3 PerlinVector::pnoise(float x,float y,float z,float w,float h,float d) 
{
	return Vector3(PerlinScalar::pnoise(x+P1x,y+P1y,z+P1z,w,h,d), 
		PerlinScalar::pnoise(x+P2x,y+P2y,z+P2z,w,h,d), 
		PerlinScalar::pnoise(x+P3x,y+P3y,z+P3z,w,h,d));
}

Vector3 PerlinVector::pnoise(float x,float y,float z,float t,float w,
					  float h,float d,float p)
{
	return Vector3(PerlinScalar::pnoise(x+P1x,y+P1y,z+P1z,t,w,h,d,p), 
		PerlinScalar::pnoise(x+P2x,y+P2y,z+P2z,t,w,h,d,p),
		PerlinScalar::pnoise(x+P3x,y+P3y,z+P3z,t,w,h,d,p));
}

Vector3 PerlinVector::pnoise(const Point2&p,float periodx,float periody)
{
	return pnoise(p.x,p.y,periodx,periody);
}

Vector3 PerlinVector::pnoise(const Point3&p,const Vector3&period) 
{
	return pnoise(p.x,p.y,p.z,period.x,period.y,period.z);
}

Vector3 PerlinVector::pnoise(const Point3&p,float t,const Vector3&pperiod,float tperiod)
{
	return pnoise(p.x,p.y,p.z,t,pperiod.x,pperiod.y,pperiod.z,tperiod);
}

Vector3 PerlinVector::spnoise(float x,float period) 
{
	return Vector3(PerlinScalar::spnoise(x+P1x,period), 
		PerlinScalar::spnoise(x+P2x,period), 
		PerlinScalar::spnoise(x+P3x,period));
}

Vector3 PerlinVector::spnoise(float x,float y,float w,float h) 
{
	return Vector3(PerlinScalar::spnoise(x+P1x,y+P1y,w,h),
		PerlinScalar::spnoise(x+P2x,y+P2y,w,h), 
		PerlinScalar::spnoise(x+P3x,y+P3y,w,h));
}

Vector3 PerlinVector::spnoise(float x,float y,float z,float w,float h,float d) 
{
	return Vector3(PerlinScalar::spnoise(x+P1x,y+P1y,z+P1z,w,h,d), 
		PerlinScalar::spnoise(x+P2x,y+P2y,z+P2z,w,h,d),
		PerlinScalar::spnoise(x+P3x,y+P3y,z+P3z,w,h,d));
}

Vector3 PerlinVector::spnoise(float x,float y,float z,float t,float w, 
							float h,float d,float p) 
{
	return Vector3(PerlinScalar::spnoise(x+P1x,y+P1y,z+P1z,t,w,h,d,p), 
		PerlinScalar::spnoise(x+P2x,y+P2y,z+P2z,t,w,h,d,p),
		PerlinScalar::spnoise(x+P3x,y+P3y,z+P3z,t,w,h,d,p));
}

Vector3 PerlinVector::spnoise(const Point2&p,float periodx,float periody)
{
	return spnoise(p.x,p.y,periodx,periody);
}

Vector3 PerlinVector::spnoise(const Point3&p,const Vector3&period)
{
	return spnoise(p.x,p.y,p.z,period.x,period.y,period.z);
}

 Vector3 PerlinVector::spnoise(const Point3&p,float t,const Vector3&pperiod,float tperiod) 
{
	return spnoise(p.x,p.y,p.z,t,pperiod.x,pperiod.y,pperiod.z,tperiod);
}