#include "StdAfx.h"

#include "BinaryFileLGAPI.h"
#include "MathUtil.h"
#include "StrUtil.h"

BinaryFileLGAPI::BinaryFileLGAPI(CString fileName)
{
	if( !m_file.Open(fileName,CFile::modeWrite | CFile::typeBinary) )
		return;	
}

void BinaryFileLGAPI::writeBoolean(boolean v) 
{
	try 
	{
		long num=v?1:0;		
	    m_file.Write(&num,sizeof(num));
	}
	catch(...)
	{		
	}
}

void BinaryFileLGAPI::writeFloat(float v)
{
	writeInt(MathUtil::floatToRawIntBits(v));
}

void BinaryFileLGAPI::writeInt(int v)
{
	try 
	{	
		int num=v&0xFF;
		m_file.Write(&num,sizeof(num));
		num=(v>>8)& 0xFF;
		m_file.Write(&num,sizeof(num));
		num=(v>>16)&0xFF;
		m_file.Write(&num,sizeof(num));
		num=(v>>24)&0xFF;
		m_file.Write(&num,sizeof(num));
	} 
	catch(...)
	{	
	}
}

void BinaryFileLGAPI::writeInterpolationType(ParameterList::InterpolationType interp)
{
	try 
	{
		CString str;
		switch(interp) 
		{
		case ParameterList::IT_NONE:
			str="n";
			m_file.Write(str,str.GetLength());
			break;
		case ParameterList::IT_VERTEX:
            str="v";
			m_file.Write(str,str.GetLength());
			break;
		case ParameterList::IT_FACE:
			str="p";
			m_file.Write(str,str.GetLength());
			break;
		case ParameterList::IT_FACEVARYING:
			str="f";
			m_file.Write(str,str.GetLength());
			break;
		default:			
			str.Format("未知插值类型\"%s\"",
				ParameterList::GetInterpolationTypeName(interp));            
		}
	} 
	catch(...)
	{		
	}
}


void BinaryFileLGAPI::writeKeyword(SCAbstractParser::SCAP_Keyword k)
{
	try 
	{
		CString str;
		switch(k) 
		{
		case SCAbstractParser::SCAP_RESET:			
			writeExtendedKeyword('R');
			break;
		case SCAbstractParser::SCAP_PARAMETER:
			str="p";
			m_file.Write(str,str.GetLength());
			break;
		case SCAbstractParser::SCAP_GEOMETRY:
			str="g";
			m_file.Write(str,str.GetLength());
			break;
		case SCAbstractParser::SCAP_INSTANCE:
			str="i";
			m_file.Write(str,str.GetLength());
			break;
		case SCAbstractParser::SCAP_SHADER:
			str="s";
			m_file.Write(str,str.GetLength());
			break;
		case SCAbstractParser::SCAP_MODIFIER:
			str="m";
			m_file.Write(str,str.GetLength());
			break;
		case SCAbstractParser::SCAP_LIGHT:
			str="l";
			m_file.Write(str,str.GetLength());
			break;
		case SCAbstractParser::SCAP_CAMERA:
			str="c";
			m_file.Write(str,str.GetLength());
			break;
		case SCAbstractParser::SCAP_OPTIONS:
			str="o";
			m_file.Write(str,str.GetLength());
			break;
		case SCAbstractParser::SCAP_INCLUDE:
			writeExtendedKeyword('i');
			break;
		case SCAbstractParser::SCAP_REMOVE:
			writeExtendedKeyword('r');
			break;
		case SCAbstractParser::SCAP_FRAME:
			writeExtendedKeyword('f');
			break;
		case SCAbstractParser::SCAP_PLUGIN:
			writeExtendedKeyword('p');
			break;
		case SCAbstractParser::SCAP_SEARCHPATH:
			writeExtendedKeyword('s');
			break;
		case SCAbstractParser::SCAP_STRING:
			writeDatatypeKeyword('s',false);
			break;
		case SCAbstractParser::SCAP_BOOL:
			writeDatatypeKeyword('b',false);
			break;
		case SCAbstractParser::SCAP_INT:
			writeDatatypeKeyword('i',false);
			break;
		case SCAbstractParser::SCAP_FLOAT:
			writeDatatypeKeyword('f',false);
			break;
		case SCAbstractParser::SCAP_COLOR:
			writeDatatypeKeyword('c',false);
			break;
		case SCAbstractParser::SCAP_POINT:
			writeDatatypeKeyword('p',false);
			break;
		case SCAbstractParser::SCAP_VECTOR:
			writeDatatypeKeyword('v',false);
			break;
		case SCAbstractParser::SCAP_TEXCOORD:
			writeDatatypeKeyword('t',false);
			break;
		case SCAbstractParser::SCAP_MATRIX:
			writeDatatypeKeyword('m',false);
			break;
		case SCAbstractParser::SCAP_STRING_ARRAY:
			writeDatatypeKeyword('s',true);
			break;
		case SCAbstractParser::SCAP_INT_ARRAY:
			writeDatatypeKeyword('i',true);
			break;
		case SCAbstractParser::SCAP_FLOAT_ARRAY:
			writeDatatypeKeyword('f',true);
			break;
		case SCAbstractParser::SCAP_POINT_ARRAY:
			writeDatatypeKeyword('p',true);
			break;
		case SCAbstractParser::SCAP_VECTOR_ARRAY:
			writeDatatypeKeyword('v',true);
			break;
		case SCAbstractParser::SCAP_TEXCOORD_ARRAY:
			writeDatatypeKeyword('t',true);
			break;
		case SCAbstractParser::SCAP_MATRIX_ARRAY:
			writeDatatypeKeyword('m',true);
			break;
		default:			
			str.Format(_T("要求未知关键词 \"%s\""), 
				SCAbstractParser::GetKeywordName(k));			
		}
	}
	catch(...)
	{		
	}
}

void BinaryFileLGAPI::writeExtendedKeyword(int code)
{
	CString str;
	str="x";
	m_file.Write(str,str.GetLength());
	m_file.Write(&code,sizeof(code));
}

void BinaryFileLGAPI::writeDatatypeKeyword(int t,boolean isArray)
{
	CString str="t";
	m_file.Write(str,str.GetLength());
	m_file.Write(&t,sizeof(t));
	writeBoolean(isArray);
}

void BinaryFileLGAPI::writeMatrix(const Matrix4&v) 
{
	vector<float> fs=v.asRowMajor();
	for(int i=0; i<fs.size(); i++)
		writeFloat(fs[i]);
}

void BinaryFileLGAPI::writeString(CString s) 
{
	try 
	{
		CString str=StrUtil::string2UTF8(s);
		writeInt(str.GetLength());
		m_file.Write(str,str.GetLength());
	} 
	catch(...) 
	{		
	}
}

void BinaryFileLGAPI::writeVerbatimString(CString s) 
{
	writeString(s);
}

void BinaryFileLGAPI::writeNewline(int indentNext) 
{	
}

void BinaryFileLGAPI::close() 
{
	try 
	{
		m_file.Close();
	} 
	catch(...) 
	{		
	}
}