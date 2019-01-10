#include "StdAfx.h"

#include "SCBinaryParser.h"
#include "MathUtil.h"
#include "StrUtil.h"

LG_IMPLEMENT_DYNCREATE(SCBinaryParser,SCAbstractParser)

void SCBinaryParser::openParser(CString fileName) 
{
	if( !m_file.Open(fileName,CFile::modeRead | CFile::typeBinary) )
		return;
	m_ar=new CArchive(&m_file,CArchive::load);
}

void SCBinaryParser::closeParser() 
{
	m_ar->Flush();
	m_ar->Close();
	if(!m_ar) delete m_ar;
	m_ar=NULL;
	m_file.Close();
}

BOOL SCBinaryParser::parseBoolean()
{
	BYTE b;
	*m_ar>>b;

	return b!= 0;
}

float SCBinaryParser::parseFloat() 
{
	return MathUtil::intBitsToFloat(parseInt());
}

int SCBinaryParser::parseInt()
{
	BYTE a,b,c,d;
	*m_ar>>a;
	*m_ar>>b;
	*m_ar>>c;
	*m_ar>>d;

	return a | (b << 8)	| (c << 16) | (d << 24);
}

Matrix4 SCBinaryParser::parseMatrix() 
{
	vector<float> data=parseFloatArray(16);

	return Matrix4(&data[0], TRUE);
}

CString SCBinaryParser::parseString() 
{
	int s=parseInt();
	char* bs;
	bs=new char[s];
	for(int i=0; i<s; i++)
	{
		char b;
		*m_ar>>b;
		bs[i]=b;
	}

	CString rt=StrUtil::string2UTF8(bs);
	delete [] bs;	

	return rt;    
}

CString SCBinaryParser::parseVerbatimString() 
{
	return parseString();
}

ParameterList::InterpolationType SCBinaryParser::parseInterpolationType() 
{
	BYTE c;
	*m_ar>>c;
	switch(c) 
	{
	case 'n':
		return ParameterList::IT_NONE;
	case 'v':
		return ParameterList::IT_VERTEX;
	case 'f':
		return ParameterList::IT_FACEVARYING;
	case 'p':
		return ParameterList::IT_FACE;
	default:
		StrUtil::PrintPrompt("插值类型发现未知的字节 %c",(char)c);
	}
}

SCAbstractParser::SCAP_Keyword SCBinaryParser::parseKeyword()
{
	int code;
	BYTE rt;
	*m_ar>>rt;
	code=rt;
	switch(code)
	{
	case 'p':
		return SCAbstractParser::SCAP_PARAMETER;
	case 'g':
		return SCAbstractParser::SCAP_GEOMETRY;
	case 'i':
		return SCAbstractParser::SCAP_INSTANCE;
	case 's':
		return SCAbstractParser::SCAP_SHADER;
	case 'm':
		return SCAbstractParser::SCAP_MODIFIER;
	case 'l':
		return SCAbstractParser::SCAP_LIGHT;
	case 'c':
		return SCAbstractParser::SCAP_CAMERA;
	case 'o':
		return SCAbstractParser::SCAP_OPTIONS;
	case 'x': 
		{	
			*m_ar>>rt;
			code=rt;
			switch(code)
			{
			case 'R':
				return SCAbstractParser::SCAP_RESET;
			case 'i':
				return SCAbstractParser::SCAP_INCLUDE;
			case 'r':
				return SCAbstractParser::SCAP_REMOVE;
			case 'f':
				return SCAbstractParser::SCAP_FRAME;
			case 'p':
				return SCAbstractParser::SCAP_PLUGIN;
			case 's':
				return SCAbstractParser::SCAP_SEARCHPATH;
			default:
				StrUtil::PrintPrompt("未知的扩展关键字编码: %c",(char)code);
			}
		}
	case 't':
		{

		int type;
		*m_ar>>rt;
		type=rt;
		BOOL isArray=parseBoolean();
		switch(type)
		{
		case 's':
			return isArray?SCAbstractParser::SCAP_STRING_ARRAY:SCAbstractParser::SCAP_STRING;
		case 'b':
			return SCAbstractParser::SCAP_BOOL;
		case 'i':
			return isArray?SCAbstractParser::SCAP_INT_ARRAY:SCAbstractParser::SCAP_INT;
		case 'f':
			return isArray?SCAbstractParser::SCAP_FLOAT_ARRAY:SCAbstractParser::SCAP_FLOAT;
		case 'c':
			return SCAbstractParser::SCAP_COLOR;
		case 'p':
			return isArray?SCAbstractParser::SCAP_POINT_ARRAY:SCAbstractParser::SCAP_POINT;
		case 'v':
			return isArray?SCAbstractParser::SCAP_VECTOR_ARRAY:SCAbstractParser::SCAP_VECTOR;
		case 't':
			return isArray?SCAbstractParser::SCAP_TEXCOORD_ARRAY:SCAbstractParser::SCAP_TEXCOORD;
		case 'm':
			return isArray?SCAbstractParser::SCAP_MATRIX_ARRAY:SCAbstractParser::SCAP_MATRIX;
		default:
			StrUtil::PrintPrompt("未知的数据类型关键字编码: %c",(char)type);
		}
	}
	default:
		if(code<0)
			return SCAbstractParser::SCAP_END_OF_FILE; 
		else
			StrUtil::PrintPrompt("未知的关键字编码: %c",(char)code);
	}
}