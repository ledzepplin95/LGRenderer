#include "StdAfx.h"
#include "IntArray.h"

IntArray::IntArray() 
{
	a.resize(10);
	s=0;
}

IntArray::IntArray(int capacity) 
{
	a.resize(capacity);
	s=0;
}

void IntArray::add(int i)
{
	if(s==a.size()) 
	{
		vector<int> oldArray=a;
		a.resize((s*3)/2+1);		
		std::copy(a.begin(),a.end(),oldArray.begin());
	}
	a[s]=i;
	s++;
}

void IntArray::set(int index,int v)
{
	a[index]=v;
}

int IntArray::get(int index) const
{
	return a[index];
}

int IntArray::getSize() const
{
	return s;
}

vector<int> IntArray::trim()
{
	if(s<a.size())
	{
		vector<int> oldArray=a;
		a.resize(s);
		std::copy(a.begin(),a.end(),oldArray.begin());
	}

	return a;
}