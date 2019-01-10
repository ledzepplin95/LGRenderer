#include "StdAfx.h"

#include "SCAbstractParser.h"
#include "strutil.h"
#include "timer.h"
#include "ColorFactory.h"
#include "lgapiinterface.h"
#include "point3.h"
#include "Vector3.h"
#include "point2.h"

LG_IMPLEMENT_DYNCREATE(SCAbstractParser,SceneParser)

BOOL SCAbstractParser::parse(CString fileName,LGAPIInterface&api)
{
	Timer timer;
	timer.start();
	StrUtil::PrintPrompt("解析\"%s\"...",fileName);
	try 
	{
		openParser(fileName);
parseloop:; 
		while(TRUE)
		{
			SCAP_Keyword k=parseKeyword();
			switch(k) 
			{
			case SCAP_RESET:
				api.reset();
				break;
			case SCAP_PARAMETER:
				parseParameter(api);
				break;
			case SCAP_GEOMETRY: 
				{
					CString name=parseString();
					CString t=parseString();
					api.geometry(name,t);
					break;
				}
			case SCAP_INSTANCE: 
				{
					CString name=parseString();
					CString geoname=parseString();
					api.instance(name,geoname);
					break;
				}
			case SCAP_SHADER: 
				{
					CString name=parseString();
					CString t=parseString();
					api.shader(name,t);
					break;
				}
			case SCAP_MODIFIER: 
				{
					CString name=parseString();
					CString t=parseString();
					api.modifier(name,t);
					break;
				}
			case SCAP_LIGHT: 
				{
					CString name=parseString();
					CString t=parseString();
					api.light(name,t);
					break;
				}
			case SCAP_CAMERA: 
				{
					CString name=parseString();
					CString t=parseString();
					api.camera(name,t);
					break;
				}
			case SCAP_OPTIONS: 
				{
					api.options(parseString());
					break;
				}
			case SCAP_INCLUDE: 
				{
					CString file=parseString();
					StrUtil::PrintPrompt("包含: \"%s\"...",file);
					api.include(file);
					break;
				}
			case SCAP_REMOVE: 
				{
					api.remove(parseString());
					break;
				}
			case SCAP_FRAME:
				{
					api.currentFrame(parseInt());
					break;
				}
			case SCAP_PLUGIN: 
				{
					CString t=parseString();
					CString name=parseString();
					CString code=parseVerbatimString();
					api.plugin(t,name,code);
					break;
				}
			case SCAP_SEARCHPATH:
				{
					CString t=parseString();
					api.searchPath(t,parseString());
					break;
				}
			case SCAP_END_OF_FILE:
				{						
					goto parseloop;
				}
			default: 
				{
					StrUtil::PrintPrompt("未知的符号%s",k);
					break;
				}
			}
		}
		closeParser();
	} 
	catch(...) 
	{			
		StrUtil::PrintPrompt("解析异常");
		return FALSE;
	}
	timer.end();
	StrUtil::PrintPrompt("解析完成(花费%s)",timer.toString());

	return TRUE;
}

void SCAbstractParser::parseParameter(LGAPIInterface&api)  
{
	CString name=parseString();
	SCAP_Keyword k=parseKeyword();
	switch(k)
	{
	case SCAP_STRING:
		{
			api.parameter(name,parseString());
			break;
		}
	case SCAP_BOOL: 
		{
			api.parameter(name,parseBoolean());
			break;
		}
	case SCAP_INT: 
		{
			api.parameter(name,parseInt());
			break;
		}
	case SCAP_FLOAT:
		{
			api.parameter(name,parseFloat());
			break;
		}
	case SCAP_COLOR: 
		{
			CString colorspace=parseString();
			int req=ColorFactory::getRequiredDataValues(colorspace);
			if(req==-2)
				api.parameter(name,colorspace);				
			else
				api.parameter(name,colorspace,parseFloatArray(req==-1?parseInt():req));
			break;
		}
	case SCAP_POINT: 
		{
			api.parameter(name,parsePoint());
			break;
		}
	case SCAP_VECTOR: 
		{
			api.parameter(name,parseVector());
			break;
		}
	case SCAP_TEXCOORD:
		{
			api.parameter(name,parseTexcoord());
			break;
		}
	case SCAP_MATRIX: 
		{
			api.parameter(name,parseMatrix());
			break;
		}
	case SCAP_STRING_ARRAY: 
		{
			int n=parseInt();
			api.parameter(name,parseStringArray(n));
			break;
		}
	case SCAP_INT_ARRAY: 
		{
			int n=parseInt();
			api.parameter(name,parseIntArray(n));
			break;
		}
	case SCAP_FLOAT_ARRAY: 
		{
			CString interp=ParameterList::GetInterpolationTypeName(parseInterpolationType());
			int n=parseInt();
			api.parameter(name,"float",interp,parseFloatArray(n));
			break;
		}
	case SCAP_POINT_ARRAY: 
		{
			CString interp=ParameterList::GetInterpolationTypeName(parseInterpolationType());
			int n=parseInt();
			api.parameter(name,"point",interp,parseFloatArray(3*n));
			break;
		}
	case SCAP_VECTOR_ARRAY:
		{
			CString interp=ParameterList::GetInterpolationTypeName(parseInterpolationType());
			int n=parseInt();
			api.parameter(name,"vector",interp,parseFloatArray(3*n));
			break;
		}
	case SCAP_TEXCOORD_ARRAY: 
		{
			CString interp=ParameterList::GetInterpolationTypeName(parseInterpolationType());
			int n=parseInt();
			api.parameter(name,"texcoord",interp,parseFloatArray(2*n));
			break;
		}
	case SCAP_MATRIX_ARRAY: 
		{
			CString interp=ParameterList::GetInterpolationTypeName(parseInterpolationType());
			int n=parseInt();
			api.parameter(name,"matrix",interp,parseMatrixArray(n));
			break;
		}
	case SCAP_END_OF_FILE:
		break;
	default: 
		{
			StrUtil::PrintPrompt("未知关键词: %s",k);
			break;
		}
	}
}

vector<CString> SCAbstractParser::parseStringArray(int size) 
{
	vector<CString> data;
	data.resize(size);
	for(int i=0; i<size; i++)
		data[i]=parseString();

	return data;
}

vector<int> SCAbstractParser::parseIntArray(int size) 
{
	vector<int> data;
	data.resize(size);
	for(int i=0; i<size; i++)
		data[i]=parseInt();

	return data;
}

vector<float> SCAbstractParser::parseFloatArray(int size) 
{
	vector<float> data;
	data.resize(size);
	for(int i=0; i<size; i++)
		data[i]=parseFloat();

	return data;
}

vector<float> SCAbstractParser::parseMatrixArray(int size)  
{
	vector<float> data;
	data.resize(16*size);
	for(int i=0, offset=0; i<size; i++, offset+=16) 
	{
		vector<float> rowdata=parseMatrix().asRowMajor();
		for(int j=0; j<16; j++)
			data[offset+j]=rowdata[j];
	}

	return data;
}

Point3 SCAbstractParser::parsePoint() 
{
	float x=parseFloat();
	float y=parseFloat();
	float z=parseFloat();

	return Point3(x,y,z);
}

Vector3 SCAbstractParser::parseVector() 
{
	float x=parseFloat();
	float y=parseFloat();
	float z=parseFloat();

	return Vector3(x,y,z);
}

Point2 SCAbstractParser::parseTexcoord() 
{
	float x=parseFloat();
	float y=parseFloat();

	return Point2(x,y);
}

CString SCAbstractParser::GetKeywordName(SCAbstractParser::SCAP_Keyword k)
{
	CString str;
	switch(k)
	{
	case SCAP_RESET:
		str="reset";
		break;
	case SCAP_PARAMETER:
		str="parameter";
		break;
	case SCAP_GEOMETRY:
		str="geometry";
		break;
	case SCAP_INSTANCE:
		str="instance";
		break;
	case SCAP_SHADER:
		str="shader";
		break;
	case SCAP_MODIFIER:
		str="modifier";
		break;
	case SCAP_LIGHT:
		str="light";
		break;
	case SCAP_CAMERA:
		str="camera";
		break;
	case SCAP_OPTIONS:
		str="options";
		break;
	case SCAP_INCLUDE:
		str="include";
		break;
	case SCAP_REMOVE:
		str="remove";
		break;
	case SCAP_FRAME:
		str="frame";
		break;
	case SCAP_PLUGIN:
		str="plugin";
		break;
	case SCAP_SEARCHPATH:
		str="searchpath";
		break;
	case SCAP_STRING:
		str="string";
		break;
	case SCAP_BOOL:
		str="bool";
		break;
	case SCAP_INT:
		str="int";
		break;
	case SCAP_FLOAT:
		str="float";
		break;
	case SCAP_COLOR:
		str="color";
		break;
	case SCAP_POINT:
		str="point";
		break;
	case SCAP_VECTOR:
		str="vector";
		break;
	case SCAP_TEXCOORD:
		str="texcoord";
		break;
	case SCAP_MATRIX:
		str="matrix";
		break;
	case SCAP_STRING_ARRAY:
		str="string_array";
		break;
	case SCAP_INT_ARRAY:
		str="int_array";
		break;
	case SCAP_FLOAT_ARRAY:
		str="float_array";
		break;
	case SCAP_POINT_ARRAY:
		str="point_array";
		break;
	case SCAP_VECTOR_ARRAY:
		str="vector_array";
		break;
	case SCAP_TEXCOORD_ARRAY:
		str="texcoord_array";
		break;
	case SCAP_MATRIX_ARRAY:
		str="matrix_array";
		break;	
	default:
		break;
	}

	return str;
}

ParameterList::InterpolationType SCAbstractParser::parseInterpolationType()
{
	return ParameterList::IT_NONE;
}

void SCAbstractParser::openParser(CString fileName)
{
}

void SCAbstractParser::closeParser()
{
}

SCAbstractParser::SCAP_Keyword SCAbstractParser::parseKeyword()
{
	return SCAP_END_OF_FILE;
}

BOOL SCAbstractParser::parseBoolean()
{
	return FALSE;
}

int SCAbstractParser::parseInt()
{
	return 0;
}

float SCAbstractParser::parseFloat()
{
	return 0.0f;
}

CString SCAbstractParser::parseString()
{
	return CString();
}

CString SCAbstractParser::parseVerbatimString()
{
	return CString();
}

Matrix4 SCAbstractParser::parseMatrix()
{
	return Matrix4();
}