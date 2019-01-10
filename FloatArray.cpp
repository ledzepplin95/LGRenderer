#include "StdAfx.h"
#include "FloatArray.h"

FloatArray::FloatArray() 
{
	a.resize(10);
	s=0;
}

FloatArray::FloatArray(int capacity) 
{
	a.resize(capacity);
	s=0;
}

void FloatArray::add(float f) 
{
	if(s==a.size()) 
	{
		vector<float> oldArray=a;
		a.resize((s*3)/2+1);
		std::copy(a.begin(),a.end(),oldArray.begin());
	}
	a[s]=f;
	s++;
}

void FloatArray::set(int index,float v)
{
	a[index]=v;
}

float FloatArray::get(int index)const
{
	return a[index];
}

int FloatArray::getSize() const
{
	return s;
}

vector<float> FloatArray::trim() 
{
	if(s<a.size())
	{
		vector<float> oldArray=a;
		a.resize(s);
		std::copy(a.begin(),a.end(),oldArray.begin());
	}

	return a;
}