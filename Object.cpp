#include "stdafx.h"

#include "object.h"
#include "point3.h"
#include "Vector3.h"
#include "matrix4.h"
#include "point2.h"
#include "color.h"
#include "LGMemoryAllocator.h"

Object::Object()
{
	m_vt=O_STRING;
	m_size=0;
	memset(&m_string[0],0,sizeof(char*)*100);		
}

Object::~Object()
{	
}

Object::Object(const CString&v)
{
	m_vt=O_STRING;
	m_string[0]=(char*)LGMemoryAllocator::lg_malloc(v.GetLength()+1);
	strcpy(m_string[0],v);
}

Object::Object(const vector<CString>&v)
{
	m_vt=O_STRING;
	m_size=v.size();
	for(size_t i=0; i<v.size(); i++)
	{
		m_string[i]=(char*)LGMemoryAllocator::lg_malloc(v[i].GetLength()+1);
		strcpy(m_string[i],v[i]);
	}
}

Object::Object(int v)
{
	m_vt=O_INT;
	m_i=v;
}

Object::Object(const vector<int>&v)
{
	m_vt=O_INT;
	m_size=v.size();
	m_int=(int*)LGMemoryAllocator::lg_malloc(sizeof(int)*m_size);
	memcpy(m_int,&v[0],sizeof(int)*m_size);
}

Object::Object(bool v)
{
	m_vt=O_BOOL;
	m_bool=v;
}

Object::Object(float v)
{
	m_vt=O_FLOAT;
	m_f=v;
}

Object::Object(const vector<float>&v)
{
	m_vt=O_FLOAT;
	m_size=v.size();
	m_float=(float*)LGMemoryAllocator::lg_malloc(sizeof(float)*m_size);
    memcpy(m_float,&v[0],sizeof(float)*m_size);
}

Object::Object(const Point3&v)
{
	m_vt=O_POINT;
	m_size=3;
	memcpy(m_float,&v.x,sizeof(Point3));
}

Object::Object(const Vector3&v)
{
	m_vt=O_VECTOR;
	m_size=3;
	memcpy(m_float,&v.x,sizeof(Vector3));
}

Object::Object(const Point2&v)
{
	m_vt=O_TEXCOORD;
	m_size=2;
	memcpy(m_float,&v.x,sizeof(Point2));
}

Object::Object(const Matrix4&v)
{
	m_vt=O_MATRIX;
	m_size=12;
	memcpy(m_float,&v,sizeof(Matrix4));
}

Object::Object(const Color&v)
{
	m_vt=O_COLOR;
	memcpy(m_color,&v,sizeof(Color));
}

Object& Object::operator=(const CString&v)
{
	m_vt=O_STRING;
	m_string[0]=(char*)LGMemoryAllocator::lg_malloc(v.GetLength()+1);
	strcpy(m_string[0],v);

	return *this;
}

Object& Object::operator=(const vector<CString>&v)
{
	m_vt=O_STRING;
	m_size=v.size();
	for(size_t i=0; i<v.size(); i++)
	{
		m_string[i]=(char*)LGMemoryAllocator::lg_malloc(v[i].GetLength()+1);
		strcpy(m_string[i],v[i]);
	}

	return *this;
}

Object& Object::operator=(int v)
{
	m_vt=O_INT;
	m_i=v;

	return *this;
}

Object& Object::operator=(const vector<int>&v)
{
	m_vt=O_INT;
	m_size=v.size();
	m_int=(int*)LGMemoryAllocator::lg_malloc(sizeof(int)*m_size);
	memcpy(m_int,&v[0],sizeof(int)*m_size);

	return *this;
}

Object& Object::operator=(bool v)
{
	m_vt=O_BOOL;
	m_bool=v;

	return *this;
}

Object& Object::operator=(float v)
{
	m_vt=O_FLOAT;
	m_f=v;

	return *this;
}

Object& Object::operator=(const vector<float>&v)
{
	m_vt=O_FLOAT;
	m_size=v.size();
	m_float=(float*)LGMemoryAllocator::lg_malloc(sizeof(float)*m_size);
	memcpy(m_int,&v[0],sizeof(int)*m_size);

	return *this;
}

Object& Object::operator=(const Point3&v)
{
	m_vt=O_POINT;
	m_size=3;
	memcpy(m_float,&v.x,sizeof(Point3));

	return *this;
}

Object& Object::operator=(const Vector3&v)
{
	m_vt=O_VECTOR;
	m_size=3;
	memcpy(m_float,&v.x,sizeof(Vector3));

	return *this;
}

Object& Object::operator=(const Point2&v)
{
	m_vt=O_TEXCOORD;
	m_size=2;
	memcpy(m_float,&v.x,sizeof(Point2));

	return *this;
}

Object& Object::operator=(const Matrix4&v)
{
	m_vt=O_MATRIX;
	m_size=12;
	memcpy(m_float,&v,sizeof(Matrix4));

	return *this;
}

Object& Object::operator=(const Color&v)
{
	m_vt=O_COLOR;
	memcpy(m_color,&v,sizeof(Color));

	return *this;
}

CString Object::AsString()const
{
	return CString(m_string[0]);
}

vector<CString> Object::AsStrings()const
{
	vector<CString> strs;
	for(int i=0;i<m_size; i++)
	{			
		char buffer[100];
		strcpy(buffer,m_string[i]);
		strs.push_back(CString(buffer));
	}

	return strs;
}

int Object::AsInt()const
{
	return m_i;
}

vector<int> Object::AsInts()const
{
	vector<int> ints;
	ints.resize(m_size);
	memcpy(&ints[0],m_int,sizeof(int)*m_size);

	return ints;
}

bool Object::AsBool()const
{
	return m_bool;
}

float Object::AsFloat()const
{
	return m_f;
}

vector<float> Object::AsFloats()const
{
	vector<float> floats;
    switch(m_vt)
	{
	case O_FLOAT:	
	case O_POINT:		
	case O_VECTOR:	
	case O_TEXCOORD:	
	case O_MATRIX:
		floats.resize(m_size);
		memcpy(&floats[0],m_float,sizeof(float)*m_size);
		break;
	}

	return floats;
}

Point3 Object::AsPoint()const
{
	return Point3(m_float[0],m_float[1],m_float[2]);
}

Vector3 Object::AsVector()const
{
	return Vector3(m_float[0],m_float[1],m_float[2]);
}

Point2 Object::AsTexCoord()const
{
	return Point2(m_float[0],m_float[1]);
}

Matrix4 Object::AsMatrix()const
{
	return Matrix4(m_float[0],m_float[1],m_float[2],
		m_float[3],m_float[4],m_float[5],
		m_float[6],m_float[7],m_float[8],
		m_float[9],m_float[10],m_float[11]);
}

Color Object::AsColor()const
{
	return Color(m_color[0],m_color[1],m_color[2]);
}