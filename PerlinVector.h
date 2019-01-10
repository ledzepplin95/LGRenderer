#pragma once

class PerlinVector
{
public:
	static Vector3 snoise(float x);
	static Vector3 snoise(float x,float y);
	static Vector3 snoise(float x,float y,float z);
	static Vector3 snoise(float x,float y,float z,float t);
	static Vector3 snoise(const Point2&p);
	static Vector3 snoise(const Point3&p);
	static Vector3 snoise(const Point3&p,float t);
	static Vector3 noise(float x);
    static Vector3 noise(float x,float y);
	static Vector3 noise(float x,float y,float z);
	static Vector3 noise(float x,float y,float z,float t);
	static Vector3 noise(const Point2&p);
	static Vector3 noise(const Point3&p);
	static Vector3 noise(const Point3&p,float t);
	static Vector3 pnoise(float x,float period);
	static Vector3 pnoise(float x,float y,float w,float h);
	static Vector3 pnoise(float x,float y,float z,float w,float h,float d);
	static Vector3 pnoise(float x,float y,float z,float t,float w,
		float h,float d,float p);
	static Vector3 pnoise(const Point2&p,float periodx,float periody);
	static Vector3 pnoise(const Point3&p,const Vector3&period);
	static Vector3 pnoise(const Point3&p,float t,const Vector3&pperiod,float tperiod);
	static Vector3 spnoise(float x,float period);
	static Vector3 spnoise(float x,float y,float w,float h);
	static Vector3 spnoise(float x,float y,float z,float w,float h,float d);
	static Vector3 spnoise(float x,float y,float z,float t,float w, 
		float h,float d,float p);
	static Vector3 spnoise(const Point2&p,float periodx,float periody);
	static Vector3 spnoise(const Point3&p,const Vector3&period);
	static Vector3 spnoise(const Point3&p,float t,const Vector3&pperiod,float tperiod);

private:
	static float P1x;
	static float P1y;
	static float P1z;
	static float P2x;
	static float P2y;
	static float P2z;
	static float P3x;
	static float P3y;
	static float P3z;	
};
