#pragma once

class PerlinScalar
{
public:
	static float snoise(float x);
	static float snoise(float x,float y);
	static float snoise(float x,float y,float z);
	static float snoise(float x,float y,float z,float w);
	static float snoise(const Point2&p);
	static float snoise(const Point3&p);
	static float snoise(const Point3&p,float t);
	static float noise(float x);
	static float noise(float x,float y);
	static float noise(float x,float y,float z) ;
	static float noise(float x,float y,float z,float t);
	static float noise(const Point2&p);
	static float noise(const Point3&p);
	static float noise(const Point3&p,float t);
	static float pnoise(float xi,float period);
	static float pnoise(float xi,float yi,float w,float h);
	static float pnoise(float xi,float yi,float zi,float w,float h,float d);
	static float pnoise(float xi,float yi,float zi,float ti,float w,
		float h,float d,float p);
	static float pnoise(const Point2&p,float periodx,float periody);
	static float pnoise(const Point3&p,const Vector3&period);
	static float pnoise(const Point3&p,float t,const Vector3&pperiod,float tperiod);
	static float spnoise(float xi,float period);
	static float spnoise(float xi,float yi,float w,float h);
	static float spnoise(float xi,float yi,float zi,float w,float h,float d);
	static float spnoise(float xi,float yi,float zi,float ti, 
		float w,float h,float d,float p);
	static float spnoise(const Point2&p,float periodx,float periody);
	static float spnoise(const Point3&p,const Vector3&period);
	static float spnoise(const Point3&p,float t,const Vector3&pperiod,float tperiod);	

private:
	static float G1[];
	static float G2[][2];
	static float G3[][3];
	static float G4[][4];
	static int p[];

    static float fade(float t);
	static float lerp(float t,float a,float b);
	static float grad(int hash,float x);
	static float grad(int hash,float x,float y);
	static float grad(int hash,float x,float y,float z);
	static float grad(int hash,float x,float y,float z,float w);
};
