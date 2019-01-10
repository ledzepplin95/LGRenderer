#include "StdAfx.h"
#include "AsciiFileLGAPI.h"

AsciiFileLGAPI::AsciiFileLGAPI(CString fileName)
{
	if(!m_file.Open(fileName,CFile::modeWrite | CFile::typeText))
		return;
}

void AsciiFileLGAPI::writeBoolean(boolean v)
{
	if(v)
		writeString("true");
	else
		writeString("false");
}

void AsciiFileLGAPI::writeFloat(float v) 
{
	CString str;
	str.Format(_T("%s"),v);
	writeString(str);
}

void AsciiFileLGAPI::writeInt(int v) 
{
	CString str;
	str.Format(_T("%d"),v);
	writeString(str);
}

void AsciiFileLGAPI::writeInterpolationType(ParameterList::InterpolationType interp)
{
	CString str;
	str.Format(_T("%s"),ParameterList::GetInterpolationTypeName(interp).MakeLower());
	writeString(str);
}

void AsciiFileLGAPI::writeKeyword(SCAbstractParser::SCAP_Keyword k) 
{
	CString scapStr=SCAbstractParser::GetKeywordName(k).MakeLower();
	scapStr.Replace("_array","[]");	
	writeString(scapStr);
}

void AsciiFileLGAPI::writeMatrix(const Matrix4&v) 
{
	writeString("row");
	vector<float> fs=v.asRowMajor();
	for(int i=0; i<fs.size(); i++)
		writeFloat(fs[i]);
}


void AsciiFileLGAPI::writeNewline(int indentNext) 
{
	try 
	{
		CString str="\n";
		m_file.Write(str,str.GetLength());
		for(int i=0; i<indentNext; i++)
		{
			str="\t";
			m_file.Write(str,str.GetLength());
		}
	} 
	catch(...) 
	{		
	}
}

void AsciiFileLGAPI::writeString(CString s) 
{
	try 
	{		
		CString str;
	    if( s.Find(" ")!=-1 && !s.Find("<code>")!=-1 )
		{
			str.Format("\"%s\"",s);
			m_file.Write(str,str.GetLength());
		}
		else
			m_file.Write(s,s.GetLength());
		str=" ";
		m_file.Write(str,str.GetLength());
	} 
	catch(...) 
	{		
	}
}

void AsciiFileLGAPI::writeVerbatimString(CString s)
{
	CString str;
	str.Format(_T("<code>%s\n</code> "),s);
	writeString(str);
}

void AsciiFileLGAPI::close() 
{
	try 
	{
		m_file.Close();
	} 
	catch(...) 
	{		
	}
}