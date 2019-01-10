#include "StdAfx.h"

#include "MovingMatrix4.h"
#include "MathUtil.h"

MovingMatrix4::MovingMatrix4(void)
{
    t0=t1=0.0f;
	inv=1.0f;
}

MovingMatrix4::~MovingMatrix4(void)
{
}

MovingMatrix4::MovingMatrix4(const Matrix4&m) 
{
	transforms.push_back(m);
	t0=t1=0.0f;
	inv=1.0f;
}

void MovingMatrix4::set(const Matrix4&m)
{
	transforms.push_back(m);
	t0=t1=0.0f;
	inv=1.0f;
}

MovingMatrix4::MovingMatrix4(int n,float tt0,float tt1,float invert)
{	
	transforms.resize(n);
	t0=tt0;
	t1=tt1;
	inv=invert;
}

void MovingMatrix4::setSteps(int n)
{
	if(transforms.size()!=n) 
	{
		transforms.resize(n);
		if(t0<t1)
			inv=(transforms.size()-1)/(t1-t0);
		else
			inv=1.0f;
	}
}

void MovingMatrix4::updateData(int i,const Matrix4&m) 
{
	transforms[i]=m;
}

Matrix4 MovingMatrix4::getData(int i)const 
{
	return transforms[i];
}

int MovingMatrix4::numSegments() const
{
	return transforms.size();
}

void MovingMatrix4::updateTimes(float tt0,float tt1) 
{
	t0=tt0;
	t1=tt1;
	if(t0<t1)
		inv=(transforms.size()-1)/(t1-t0);
	else
		inv=1.0f;
}

MovingMatrix4 MovingMatrix4::inverse() 
{
	MovingMatrix4 mi(transforms.size(),t0,t1,inv);
	for(int i=0; i<transforms.size(); i++)
	{
		if(transforms[i]!=Matrix4())
		{
			mi.transforms[i]=transforms[i].inverse();
			if(mi.transforms[i]==Matrix4())
				return MovingMatrix4();
		}	
	}

	return mi;
}

Matrix4 MovingMatrix4::sample(float time) const
{
	if( transforms.size()==1 
		|| t0>=t1 )
		return transforms[0];
	else 
	{
		float nt=(MathUtil::clamp(time,t0,t1)-t0)*inv;
		int idx0=(int)nt;
		int idx1=min(idx0+1,transforms.size()-1);

		return Matrix4::blend(transforms[idx0], 
			transforms[idx1],(float)(nt-idx0));
	}
}

BOOL MovingMatrix4::isNull()const
{
	return transforms.empty();
}

MovingMatrix4& MovingMatrix4::operator =(const MovingMatrix4 &mm)
{
	if(this==&mm) return *this;

	transforms=mm.transforms;
	t0=mm.t0;
	t1=mm.t1;
	inv=mm.inv;

	return *this;
}

