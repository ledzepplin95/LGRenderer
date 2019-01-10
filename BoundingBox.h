#pragma once

#include "point3.h"

class BoundingBox
{
public:
	BoundingBox(void);
	~BoundingBox(void);
	BoundingBox(const BoundingBox&b);
	BoundingBox(const Point3&p);
	BoundingBox(float x,float y,float z);
	BoundingBox(float size);

	void set(const BoundingBox&b);
	void set(const Point3&p);
	void set(float x,float y,float z);
	void set(float size);

	Point3 getMinimum()const;
	Point3 getMaximum()const;
	Point3 getCenter()const;
	Point3 getCorner(int i)const;
	float getBound(int i)const;
	Vector3 getExtents()const;
	float getArea()const;
	float getVolume()const;
	void enlargeUlps();
	BOOL isEmpty()const;
    BOOL intersects(const BoundingBox&b)const;
	BOOL contains(const Point3&p)const;
	BOOL contains(float x,float y,float z)const;

	Point3& Maximum();
	Point3& Minimum();

	void include(const Point3&p);
	void include(float x,float y,float z);
	void include(const BoundingBox&b);
	CString toString()const; 
    BoundingBox& operator=(const BoundingBox&b);
	BOOL operator!=(const BoundingBox&b) const;
    BOOL operator==(const BoundingBox&b) const;

private:
	Point3 minimum;
	Point3 maximum;
};
