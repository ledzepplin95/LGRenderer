#include "StdAfx.h"

#include "StrUtil.h"
#include <boost/regex.hpp>
#include "vector3.h"
#include "color.h"
#include "point3.h"

vector<CString> StrUtil::split(CString cs,CString regex_str)
{
	vector<CString> tokens;
	string s(cs);
	boost::regex re(regex_str);
	boost::sregex_token_iterator i(s.begin(),s.end(),re,-1);
	boost::sregex_token_iterator j;

	while(i!=j)
	{
		string ss=(*i++).str();
		tokens.push_back(ss.c_str());		
	}

	return tokens;
}

CString StrUtil::subString(CString source,int start,int end)
{
	int e=end;
	if(end==-1) e=source.GetLength()-1;
    CString str;
    for(int i=start; i<e; i++)
		str.AppendChar(source.GetAt(i));

    return str;
}

CString StrUtil::GetRandomFileName()
{
	srand((unsigned)time(NULL));
	int ranNum=rand();
	char buffer[20];
	_itoa_s(ranNum,buffer,20,10);
	CString fileName="LG";
	fileName+=buffer;
	fileName+=".lg";

	return fileName;
}

CString StrUtil::GetFileDir(CString fileName)
{
	TCHAR drive[_MAX_DRIVE];
	TCHAR dir[_MAX_DIR];
	TCHAR fname[_MAX_FNAME];
	TCHAR ext[_MAX_EXT];	
	_splitpath_s(fileName,drive,dir,fname,ext);

	CString str;
	str=drive;
	str+=dir;

	return str;
}

CString StrUtil::string2UTF8(CString s)
{
	char bs[1000];
	strcpy_s(bs,s);
	int len=MultiByteToWideChar(CP_ACP,0,bs,-1,NULL,0);
	wchar_t *pwText=new wchar_t[len];
	len=MultiByteToWideChar(CP_ACP,0,bs,-1,pwText,len);

	len=WideCharToMultiByte(CP_UTF8,0,pwText,-1,NULL,0,NULL,NULL);
	len=WideCharToMultiByte(CP_UTF8,NULL,pwText,-1,bs,len,NULL,NULL);
	delete [] pwText;

	return CString(bs);
}

void StrUtil::PrintPrompt(const char*fmt,...)
{
	try
	{
		va_list vlist;
		va_start(vlist,fmt);
		char buf[800];
		vsprintf(buf,fmt,vlist);
		va_end(vlist);
		::OutputDebugString(buf);
		::OutputDebugString("\n");
	}
	catch(...)
	{
	}
}

CString StrUtil::trimExtensionLeft(CString s)
{
	CString ext=s.MakeLower();
    if(ext.Find(".")==0) ext.Delete(0,1);

	return ext;
}

CString StrUtil::GetModuleFilePath()
{
	TCHAR szPath[MAX_PATH]; 
	if(!::GetModuleFileName(NULL,szPath,MAX_PATH))
		return CString();
	CString str(szPath);

	return str;
}

void StrUtil::dpo(double n)
{
	char buffer[50];
	_gcvt(n,10,buffer);
	::OutputDebugString(buffer);
}

void StrUtil::dpo(CString s)
{
	::OutputDebugString(s);
}

void StrUtil::dpo(const Vector3&v)
{
	CString s;
	s.Format(_T("%.2f %.2f %.2f"),v.x,v.y,v.z);
	::OutputDebugString(s);
}

void StrUtil::dpo(const Point3&p)
{
	CString s;
	s.Format(_T("%.2f %.2f %.2f"),p.x,p.y,p.z);
	::OutputDebugString(s);
}

void StrUtil::dpo(const Color&c)
{
	CString s;
	s.Format(_T("%.2f %.2f %.2f"),c.getRed(),c.getGreen(),c.getBlue());
	::OutputDebugString(s);
}



