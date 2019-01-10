#include "StdAfx.h"

#include "FileLGAPI.h"
#include "strutil.h"
#include "ColorFactory.h"
#include "point3.h"
#include "Vector3.h"
#include "point2.h"

FileLGAPI::FileLGAPI() 
{
	frame=1;
	reset();
}

void FileLGAPI::camera(CString name,CString lensType) 
{
	writeKeyword(SCAbstractParser::SCAP_CAMERA);
	writeString(name);
	writeString(lensType);
	writeNewline(0);
	writeNewline(0);
}

void FileLGAPI::geometry(CString name,CString typeName) 
{
	writeKeyword(SCAbstractParser::SCAP_GEOMETRY);
	writeString(name);
	writeString(typeName);
	writeNewline(0);
	writeNewline(0);
}

int FileLGAPI::getCurrentFrame()
{
	return frame;
}

void FileLGAPI::instance(CString name,CString geoname) 
{
	writeKeyword(SCAbstractParser::SCAP_INSTANCE);
	writeString(name);
	writeString(geoname);
	writeNewline(0);
	writeNewline(0);
}

void FileLGAPI::light(CString name,CString lightType)
{
	writeKeyword(SCAbstractParser::SCAP_LIGHT);
	writeString(name);
	writeString(lightType);
	writeNewline(0);
	writeNewline(0);
}

void FileLGAPI::modifier(CString name,CString modifierType) 
{
	writeKeyword(SCAbstractParser::SCAP_MODIFIER);
	writeString(name);
	writeString(modifierType);
	writeNewline(0);
	writeNewline(0);
}

void FileLGAPI::options(CString name) 
{
	writeKeyword(SCAbstractParser::SCAP_OPTIONS);
	writeString(name);
	writeNewline(0);
	writeNewline(0);
}

void FileLGAPI::parameter(CString name,CString v) 
{
	writeKeyword(SCAbstractParser::SCAP_PARAMETER);
	writeString(name);
	writeKeyword(SCAbstractParser::SCAP_STRING);
	writeString(v);
	writeNewline(0);
}

void FileLGAPI::parameter(CString name,boolean v)
{
	writeKeyword(SCAbstractParser::SCAP_PARAMETER);
	writeString(name);
	writeKeyword(SCAbstractParser::SCAP_BOOL);
	writeBoolean(v);
	writeNewline(0);
}

void FileLGAPI::parameter(CString name,int v) 
{
	writeKeyword(SCAbstractParser::SCAP_PARAMETER);
	writeString(name);
	writeKeyword(SCAbstractParser::SCAP_INT);
	writeInt(v);
	writeNewline(0);
}

void FileLGAPI::parameter(CString name,float v) 
{
	writeKeyword(SCAbstractParser::SCAP_PARAMETER);
	writeString(name);
	writeKeyword(SCAbstractParser::SCAP_FLOAT);
	writeFloat(v);
	writeNewline(0);
}

void FileLGAPI::parameter(CString name,CString colorSpace,const vector<float>&data) 
{
	writeKeyword(SCAbstractParser::SCAP_PARAMETER);
	writeString(name);
	writeKeyword(SCAbstractParser::SCAP_COLOR);
	if(colorSpace.IsEmpty())
		writeString(colorSpace=ColorFactory::getInternalColorSpace());
	else
		writeString(colorSpace);
	if(ColorFactory::getRequiredDataValues(colorSpace)==-1)
		writeInt(data.size());
	int idx=0;
	int step=9;
	for(int f=0; f<data.size(); f++) 
	{
		if( data.size()>step && idx%step==0 )
			writeNewline(1);
		writeFloat(data[f]);
		idx++;
	}
	writeNewline(0);
}

void FileLGAPI::parameter(CString name,const Point3&v) 
{
	writeKeyword(SCAbstractParser::SCAP_PARAMETER);
	writeString(name);
	writeKeyword(SCAbstractParser::SCAP_POINT);
	writeFloat(v.x);
	writeFloat(v.y);
	writeFloat(v.z);
	writeNewline(0);
}

void FileLGAPI::parameter(CString name,const Vector3&v)
{
	writeKeyword(SCAbstractParser::SCAP_PARAMETER);
	writeString(name);
	writeKeyword(SCAbstractParser::SCAP_VECTOR);
	writeFloat(v.x);
	writeFloat(v.y);
	writeFloat(v.z);
	writeNewline(0);
}

void FileLGAPI::parameter(CString name,const Point2&v) 
{
	writeKeyword(SCAbstractParser::SCAP_PARAMETER);
	writeString(name);
	writeKeyword(SCAbstractParser::SCAP_TEXCOORD);
	writeFloat(v.x);
	writeFloat(v.y);
	writeNewline(0);
}

void FileLGAPI::parameter(CString name,const Matrix4&v)
{
	writeKeyword(SCAbstractParser::SCAP_PARAMETER);
	writeString(name);
	writeKeyword(SCAbstractParser::SCAP_MATRIX);
	writeMatrix(v);
	writeNewline(0);
}

void FileLGAPI::parameter(CString name,const vector<int>&v) 
{
	writeKeyword(SCAbstractParser::SCAP_PARAMETER);
	writeString(name);
	writeKeyword(SCAbstractParser::SCAP_INT_ARRAY);
	writeInt(v.size());
	int idx=0;
	int step=9;
	for(int i=0; i<v.size(); i++) 
	{
		if(idx%step==0)
			writeNewline(1);
		writeInt(v[i]);
		idx++;
	}
	writeNewline(0);
}

void FileLGAPI::parameter(CString name,const vector<CString>&v)
{
	writeKeyword(SCAbstractParser::SCAP_PARAMETER);
	writeString(name);
	writeKeyword(SCAbstractParser::SCAP_STRING_ARRAY);
	writeInt(v.size());
	for(int i=0; i<v.size(); i++)
	{
		writeNewline(1);
		writeString(v[i]);
	}
	writeNewline(0);
}

ParameterList::InterpolationType FileLGAPI::GetInterpolationType(const char*interpolation)
{
	return (ParameterList::InterpolationType)atoi(interpolation);    
}

void FileLGAPI::parameter(CString name,CString t,CString interpolation,
						  const vector<float>&data)
{
	ParameterList::InterpolationType interp;
	try 
	{
		interp=GetInterpolationType(interpolation.MakeUpper());
	}
	catch(...)
	{
		StrUtil::PrintPrompt("未知的插值类型: %s -- 忽略参数\"%s\"", 
			interpolation,name);
		return;
	}
	SCAbstractParser::SCAP_Keyword typeKeyword;
	int lengthFactor;
	if(t=="float")
	{
		typeKeyword=SCAbstractParser::SCAP_FLOAT_ARRAY;
		lengthFactor=1;
	}
	else if(t=="point") 
	{
		typeKeyword=SCAbstractParser::SCAP_POINT_ARRAY;
		lengthFactor=3;
	} 
	else if(t=="vector") 
	{
		typeKeyword=SCAbstractParser::SCAP_VECTOR_ARRAY;
		lengthFactor=3;
	} 
	else if(t=="texcoord")
	{
		typeKeyword=SCAbstractParser::SCAP_TEXCOORD_ARRAY;
		lengthFactor=2;
	} 
	else if(t=="matrix") 
	{
		typeKeyword=SCAbstractParser::SCAP_MATRIX_ARRAY;
		lengthFactor=16;
	} 
	else 
	{
		StrUtil::PrintPrompt("未知的参数类型: %s -- 忽略参数 \"%s\"", 
			t,name);
		return;
	}
	writeKeyword(SCAbstractParser::SCAP_PARAMETER);

	writeString(name);
	writeKeyword(typeKeyword);
	writeInterpolationType(interp);
	writeInt(data.size()/lengthFactor);
	int idx=0;
	if(data.size()>16)
		lengthFactor*= 8;
	for(int v=0; v<data.size() ; v++)
	{
		if( lengthFactor>1 && idx%lengthFactor==0 )
			writeNewline(1);
		writeFloat(data[v]);
		idx++;
	}
	writeNewline(0);
}

BOOL FileLGAPI::include(CString fileName) 
{
	writeKeyword(SCAbstractParser::SCAP_INCLUDE);
	writeString(fileName);
	writeNewline(0);
	writeNewline(0);

	return TRUE;
}

void FileLGAPI::plugin(CString t,CString name,CString code)
{
	writeKeyword(SCAbstractParser::SCAP_PLUGIN);
	writeString(t);
	writeString(name);
	writeVerbatimString(code);
	writeNewline(0);
	writeNewline(0);
}

void FileLGAPI::remove(CString name)
{
	writeKeyword(SCAbstractParser::SCAP_REMOVE);
	writeString(name);
	writeNewline(0);
	writeNewline(0);
}

void FileLGAPI::render(CString optionsName,Display*display) 
{
	StrUtil::PrintPrompt("不能渲染文件流");
}

void FileLGAPI::reset()
{
	frame=1;
}

void FileLGAPI::searchPath(CString t,CString path) 
{
	writeKeyword(SCAbstractParser::SCAP_SEARCHPATH);
	writeString(t);
	writeString(path);
	writeNewline(0);
	writeNewline(0);
}

void FileLGAPI::currentFrame(int cf) 
{
	writeKeyword(SCAbstractParser::SCAP_FRAME);
	writeInt(frame=cf);
	writeNewline(0);
	writeNewline(0);
}

void FileLGAPI::shader(CString name,CString shaderType) 
{
	writeKeyword(SCAbstractParser::SCAP_SHADER);
	writeString(name);
	writeString(shaderType);
	writeNewline(0);
	writeNewline(0);
}