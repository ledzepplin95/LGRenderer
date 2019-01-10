#pragma once

class Point3
{
public:
	Point3(void);
	~Point3(void);
	Point3(float xx,float yy,float zz);
	Point3(const Point3&p);

	float get(int i)const;
	float distanceTo(const Point3&p)const;
	float distanceTo(float px,float py,float pz)const;
	float distanceToSquared(const Point3&p)const;
	float distanceToSquared(float px,float py,float pz)const;
	CString toString()const;
	BOOL operator!=(const Point3&p)const;
	BOOL operator==(const Point3&p)const;

	Point3& set(float xx,float yy,float zz);
	Point3& set(const Point3&p);	
	Point3& operator=(const Point3&p);
	Vector3 operator-(const Point3&p);

	static Point3 add(const Point3&p,const Vector3&v,Point3&dest);
	static Vector3 sub(const Point3&p1,const Point3&p2,Vector3&dest);
	static Point3 mid(const Point3&p1,const Point3&p2,Point3&dest);
	static Point3 blend(const Point3&p0,const Point3&p1,float b,Point3&dest);
	static Vector3 normal(const Point3&p0,const Point3&p1,const Point3&p2);
	static Vector3 normal(const Point3&p0,const Point3&p1,const Point3&p2,Vector3&dest);

	float x,y,z;
};
