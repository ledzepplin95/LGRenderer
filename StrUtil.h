#pragma once

class StrUtil
{
public:
    static vector<CString> split(CString cs,CString regex_str="\\s+");
    static CString subString(CString source,int start,int end=-1);
	static CString GetRandomFileName();
	static CString GetFileDir(CString fileName);
	static CString string2UTF8(CString s);
	static void PrintPrompt(const char*fmt,...);
	static CString trimExtensionLeft(CString s);
	static CString GetModuleFilePath();
    static void dpo(CString s);
	static void dpo(double n);
	static void dpo(const Vector3&v);
	static void dpo(const Point3&p);
	static void dpo(const Color&c);
};
