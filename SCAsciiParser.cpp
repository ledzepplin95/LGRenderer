#include "StdAfx.h"

#include "SCAsciiParser.h"
#include "strutil.h"
#include "MathUtil.h"
#include "color.h"

LG_IMPLEMENT_DYNCREATE(SCAsciiParser,SCAbstractParser)

Color SCAsciiParser::parseColor() 
{
	CString space=p.getNextToken();
	Color c;
	if(space=="sRGB nonlinear")
	{
		float r=p.getNextFloat();
		float g=p.getNextFloat();
		float b=p.getNextFloat();
		c.set(r,g,b);
		c.toLinear();
	}
	else if(space=="sRGB linear") 
	{
		float r=p.getNextFloat();
		float g=p.getNextFloat();
		float b=p.getNextFloat();
		c.set(r,g,b);
	}
	else
		StrUtil::PrintPrompt("未知的颜色空间: %s",space);

	return c;
}

Matrix4 SCAsciiParser::parseMatrix()
{
	if(p.peekNextToken("row"))
	{
		vector<float> data=parseFloatArray(12);

		return Matrix4(&data[0],TRUE);
	} 
	else if(p.peekNextToken("col"))
	{
		vector<float> data=parseFloatArray(12);

		return Matrix4(&data[0],FALSE);
	}
	else
	{
		Matrix4 m=Matrix4::IDENTITY;
		try 
		{
			p.checkNextToken("{");
		} 
		catch (...)
		{
			return Matrix4();
		}
		while(!p.peekNextToken("}"))
		{
			Matrix4 t;
			if(p.peekNextToken("translate"))
			{
				float x=p.getNextFloat();
				float y=p.getNextFloat();
				float z=p.getNextFloat();
				t=Matrix4::translation(x,y,z);
			} 
			else if(p.peekNextToken("scaleu"))
			{
				float s=p.getNextFloat();
				t=Matrix4::scale(s);
			} 
			else if(p.peekNextToken("scale"))
			{
				float x=p.getNextFloat();
				float y=p.getNextFloat();
				float z=p.getNextFloat();
				t=Matrix4::scale(x,y,z);
			} 
			else if(p.peekNextToken("rotatex"))
			{
				float angle=p.getNextFloat();
				t=Matrix4::rotateX((float)DegToRad(angle));
			} 
			else if(p.peekNextToken("rotatey")) 
			{
				float angle=p.getNextFloat();
				t=Matrix4::rotateY((float)DegToRad(angle));
			} 
			else if(p.peekNextToken("rotatez")) 
			{
				float angle=p.getNextFloat();
				t=Matrix4::rotateZ((float)DegToRad(angle));
			}
			else if(p.peekNextToken("rotate"))
			{
				float x=p.getNextFloat();
				float y=p.getNextFloat();
				float z=p.getNextFloat();
				float angle=p.getNextFloat();
				t=Matrix4::rotate(x,y,z,(float)DegToRad(angle));
			}
			else
				StrUtil::PrintPrompt("未知的变换类型: %s",
			p.getNextToken());			
			m=t.multiply(m);
		}

		return m;
	}
}

void SCAsciiParser::closeParser()
{
	p.close();
}

void SCAsciiParser::openParser(CString fileName)
{
	p.Set(fileName);
}

BOOL SCAsciiParser::parseBoolean() 
{
	return atoi(parseString());
}

float SCAsciiParser::parseFloat() 
{
	return p.getNextFloat();
}

int SCAsciiParser::parseInt() 
{
	return p.getNextInt();
}

CString SCAsciiParser::parseString()
{
	return p.getNextToken();
}

CString SCAsciiParser::parseVerbatimString()
{
	try 
	{
		return p.getNextCodeBlock();
	} 
	catch(...)
	{	
		return CString();
	}
}

ParameterList::InterpolationType SCAsciiParser::parseInterpolationType() 
{
	if(p.peekNextToken("none"))
		return ParameterList::IT_NONE;
	else if(p.peekNextToken("vertex"))
		return ParameterList::IT_VERTEX;
	else if(p.peekNextToken("face"))
		return ParameterList::IT_FACE;
	else if(p.peekNextToken("facevarying"))
		return ParameterList::IT_FACEVARYING;

	return ParameterList::IT_NONE;
}

SCAbstractParser::SCAP_Keyword SCAsciiParser::parseKeyword() 
{
	CString keyword=p.getNextToken();
	if(keyword.IsEmpty())
		return SCAbstractParser::SCAP_END_OF_FILE;
	if(anyEqual(keyword,1,"reset"))
		return SCAbstractParser::SCAP_RESET;
	if(anyEqual(keyword,3,"parameter","param","p"))
		return SCAbstractParser::SCAP_PARAMETER;
	if(anyEqual(keyword,3,"geometry","geom","g"))
		return SCAbstractParser::SCAP_GEOMETRY;
	if(anyEqual(keyword,3,"instance","inst","i"))
		return SCAbstractParser::SCAP_INSTANCE;
	if(anyEqual(keyword,3,"shader","shd","s"))
		return SCAbstractParser::SCAP_SHADER;
	if(anyEqual(keyword,3,"modifier","mod","m"))
		return SCAbstractParser::SCAP_MODIFIER;
	if(anyEqual(keyword,2,"light","l"))
		return SCAbstractParser::SCAP_LIGHT;
	if(anyEqual(keyword,3,"camera","cam","c"))
		return SCAbstractParser::SCAP_CAMERA;
	if(anyEqual(keyword,3,"options","opt","o"))
		return SCAbstractParser::SCAP_OPTIONS;
	if(anyEqual(keyword,2,"include","inc"))
		return SCAbstractParser::SCAP_INCLUDE;
	if(anyEqual(keyword,1,"remove"))
		return SCAbstractParser::SCAP_REMOVE;
	if(anyEqual(keyword,1,"frame"))
		return SCAbstractParser::SCAP_FRAME;
	if(anyEqual(keyword, 2,"plugin","plug"))
		return SCAbstractParser::SCAP_PLUGIN;
	if(anyEqual(keyword, 1,"searchpath"))
		return SCAbstractParser::SCAP_SEARCHPATH;
	if(anyEqual(keyword, 2,"string","str"))
		return SCAbstractParser::SCAP_STRING;
	if(anyEqual(keyword, 2,"string[]","str[]"))
		return SCAbstractParser::SCAP_STRING_ARRAY;
	if(anyEqual(keyword, 2,"boolean","bool"))
		return SCAbstractParser::SCAP_BOOL;
	if(anyEqual(keyword, 2,"integer","int"))
		return SCAbstractParser::SCAP_INT;
	if(anyEqual(keyword, 2,"integer[]","int[]"))
		return SCAbstractParser::SCAP_INT_ARRAY;
	if(anyEqual(keyword, 2,"float","flt"))
		return SCAbstractParser::SCAP_FLOAT;
	if(anyEqual(keyword, 2,"float[]","flt[]"))
		return SCAbstractParser::SCAP_FLOAT_ARRAY;
	if(anyEqual(keyword, 2,"color", "col"))
		return SCAbstractParser::SCAP_COLOR;
	if(anyEqual(keyword, 2,"point","pnt"))
		return SCAbstractParser::SCAP_POINT;
	if(anyEqual(keyword, 2,"point[]","pnt[]"))
		return SCAbstractParser::SCAP_POINT_ARRAY;
	if(anyEqual(keyword, 2,"vector","vec"))
		return SCAbstractParser::SCAP_VECTOR;
	if(anyEqual(keyword, 2,"vector[]","vec[]"))
		return SCAbstractParser::SCAP_VECTOR_ARRAY;
	if(anyEqual(keyword, 2,"texcoord","tex"))
		return SCAbstractParser::SCAP_TEXCOORD;
	if(anyEqual(keyword, 2,"texcoord[]","tex[]"))
		return SCAbstractParser::SCAP_TEXCOORD_ARRAY;
	if(anyEqual(keyword, 2,"matrix","mat"))
		return SCAbstractParser::SCAP_MATRIX;
	if(anyEqual(keyword, 2,"matrix[]","mat[]"))
		return SCAbstractParser::SCAP_MATRIX_ARRAY;

	return SCAbstractParser::SCAP_END_OF_FILE;
}


BOOL SCAsciiParser::anyEqual(const CString&source,long num,CString first,...)
{
	long count=num-1;	
	va_list v1;

	va_start(v1,first);
	while(count--)
	{
		CString comp=va_arg(v1,CString);
		if(comp==source) return TRUE;
	}
	va_end(v1);

	return FALSE;
}