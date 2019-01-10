#include "StdAfx.h"

#include "Memory.h"

CString Memory::m_sizeof(const vector<int>&a) 
{
	return bytesToString(sizeof(int)*a.size());
}

CString Memory::bytesToString(long bytes) 
{
	CString str;
	if(bytes<1024)
	{		
		str.Format(_T("%db"),bytes);

		return str;
	}
	if(bytes<1024*1024)
	{      
	    str.Format(_T("%dKb"),(bytes+512)>>10);

		return str;
	}
	str.Format(_T("%dMb"),(bytes+512*1024)>>20);

	return str;
}
