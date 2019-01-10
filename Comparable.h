#pragma once

template <class T> class  Comparable
{
public:
	virtual int compareTo(T o)
	{
		return 0;
	}	
};
