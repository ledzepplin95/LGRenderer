#pragma once

class Ray
{
public:
	Ray(void);

public:
    Ray(float oox,float ooy,float ooz,float ddx,float ddy,float ddz);
	Ray(const Point3&o,const Vector3&d);
	Ray(const Point3&a,const Point3&b);

	void normalize();
	void setMax(float t);
	void reverseDirection();
	Ray& operator=(const Ray&r);

	Ray transform(const Matrix4&m)const;	
	float getMin()const;
	float getMax()const;
	Vector3 getDirection()const;
	BOOL isInside(float t)const;
	Point3 getPoint(Point3&dest)const;
	float dot(const Vector3&v)const;
	float dot(float vx,float vy,float vz)const;
	BOOL operator==(const Ray&r)const;
	BOOL operator!=(const Ray&r)const;
	BOOL isNull() const;

	float ox,oy,oz;
	float dx,dy,dz;

private:    
	float tMin;
	float tMax;

	static float EPSILON;	
};
