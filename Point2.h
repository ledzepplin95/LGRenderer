#pragma once

class Point2
{
public:
	Point2(void);
	~Point2(void);
	Point2(float xx,float yy);
	Point2(const Point2&p);

	Point2& set(float xx,float yy);
	Point2& set(const Point2&p);	
	Point2& operator=(const Point2&p);

	CString toString()const;
	BOOL operator==(const Point2&p)const;
	BOOL operator!=(const Point2&p)const;

	float x,y;
};
