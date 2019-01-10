#pragma once

#include "matrix4.h"

class MovingMatrix4
{
    MovingMatrix4(int n,float tt0,float tt1,float invert);
public:
	MovingMatrix4(void);
	~MovingMatrix4(void);
	MovingMatrix4(const Matrix4&m);

	MovingMatrix4& operator=(const MovingMatrix4&mm);
    void set(const Matrix4&m);
	void setSteps(int n);
	void updateData(int i,const Matrix4&m);	
	void updateTimes(float tt0,float tt1);
	MovingMatrix4 inverse();

	int numSegments()const;
	Matrix4 getData(int i)const;
    Matrix4 sample(float time)const;
	BOOL isNull()const;

private:
    vector<Matrix4> transforms;
	float t0,t1,inv;
};
