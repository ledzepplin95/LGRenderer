#include "StdAfx.h"

#include "ParameterList.h"
#include "strutil.h"
#include "point3.h"
#include "Vector3.h"
#include "point2.h"
#include "color.h"

LG_IMPLEMENT_DYNCREATE(ParameterList,LGObject)

ParameterList::Parameter::Parameter()
{
}

ParameterList::FloatParameter::FloatParameter()
{	
	interp=IT_NONE;
}

ParameterList::FloatParameter::FloatParameter(float f) 
{	
	interp=IT_NONE;
    data.assign(1,f);
}

ParameterList::FloatParameter::FloatParameter(InterpolationType it,
											  const vector<float>&d)
{
	interp=it;
	data=d;
}

ParameterList::FloatParameter& ParameterList::FloatParameter::operator=(
	const FloatParameter&fp)
{
	if(this==&fp) return *this;

	interp=fp.interp;
	data=fp.data;

	return *this;
}

BOOL ParameterList::FloatParameter::isNull() const
{
	return data.empty() && interp==IT_NONE;
}

ParameterList::Parameter::Parameter(const Parameter&p)
{
	if(this==&p) return;

	t=p.t;
	interp=p.interp;
	obj=p.obj;
	checked=p.checked;
}

ParameterList::Parameter::Parameter(CString v) 
{
	t=PT_STRING;
	interp=IT_NONE;
	obj=CString(v);
	checked=FALSE;
}

ParameterList::Parameter::Parameter(int v) 
{
	t=PT_INT;
	interp=IT_NONE;
	obj=(int)v;
	checked=FALSE;
}

ParameterList::Parameter::Parameter(bool v) 
{
	t=PT_BOOL;
	interp=IT_NONE;
	obj=(bool)v;
	checked=FALSE;
}

ParameterList::Parameter::Parameter(float v)
{
	t=PT_FLOAT;
	interp=IT_NONE;
	obj=(float)v;
	checked=FALSE;
}

ParameterList::Parameter::Parameter(const vector<int>&a) 
{
	t=PT_INT;
	interp=IT_NONE;
	obj=a;
	checked=FALSE;
}

ParameterList::Parameter::Parameter(const vector<CString>&a) 
{
	t=PT_STRING;
	interp=IT_NONE;
	obj=a;
	checked=FALSE;
}

ParameterList::Parameter::Parameter(const Color&c) 
{
	t=PT_COLOR;
	interp=IT_NONE;
	obj=c;
	checked=FALSE;
}

ParameterList::Parameter::Parameter(ParameterType tt, 
	InterpolationType it,const vector<float>&d) 
{
	t=tt;
	interp=it;
	obj=d;
	switch(tt)
	{
	case PT_FLOAT:
		break;
	case PT_POINT:
		obj.m_vt=Object::O_POINT;
		break;
	case PT_VECTOR:
	    obj.m_vt=Object::O_VECTOR;
		break;
	case PT_TEXCOORD:
	    obj.m_vt=Object::O_TEXCOORD;
		break;
	case PT_MATRIX:
	    obj.m_vt=Object::O_MATRIX;
		break;
	}
	checked=FALSE;
}

int ParameterList::Parameter::size() const
{	
	switch(t) 
	{
	case PT_STRING:
		return sizeof(obj);
	case PT_INT:
		return sizeof(obj);
	case PT_BOOL:
		return 1;
	case PT_FLOAT:
		return sizeof(obj);
	case PT_POINT:
		return sizeof(obj)/3;
	case PT_VECTOR:
		return sizeof(obj)/3;
	case PT_TEXCOORD:
		return sizeof(obj)/2;
	case PT_MATRIX:
		return sizeof(obj)/12;
	case PT_COLOR:
		return 1;
	default:
		return -1;
	}
}

void ParameterList::Parameter::check() 
{
	checked=TRUE;
}

CString ParameterList::GetParameterTypeName(ParameterList::ParameterType ptt)
{
    CString str="unknown";
	switch(ptt)
	{
	case PT_STRING:
		str="string";
		break;
	case PT_INT:
		str="int";
		break;
	case PT_BOOL:
		str="bool";
		break;
	case PT_FLOAT:
		str="float";
		break;
	case PT_POINT:
		str="point";
		break;
	case PT_VECTOR:
		str="vector";
		break;
	case PT_TEXCOORD:
		str="texcoord";
		break;
	case PT_MATRIX:
		str="matrix";
		break;
	case PT_COLOR:
		str="color";
		break;
	default:
		break;
	}

	return str;
}

ParameterList::InterpolationType ParameterList::GetInterpolationType(CString t)
{
	InterpolationType it=IT_NONE;
	CString tt=t.MakeLower();

	if(tt=="face")
		it=IT_FACE;
	else if(tt=="vertex")
		it=IT_VERTEX;
	else if(tt=="facevarying")
		it=IT_FACEVARYING;

    return it;
}

CString ParameterList::GetInterpolationTypeName(ParameterList::InterpolationType itt)
{
	CString str;
	switch(itt)
	{
	case IT_NONE:
		str="none";
		break;
	case IT_FACE:
		str="face";
		break;
	case IT_VERTEX:
		str="vertex";
		break;
	case IT_FACEVARYING:
		str="facevarying";
		break;
	default:
		break;
	}

	return str;
}

CString ParameterList::Parameter::toString() const
{
	CString str;
	CString str1=ParameterList::GetInterpolationTypeName(interp);
	CString str2=ParameterList::GetParameterTypeName(t);
	str1+=" "+str2;
	str.Format(_T("%s%s[%d]"),interp==IT_NONE ?"":str2,size());

	return str;
}

CString ParameterList::Parameter::getStringValue() const
{	
	return obj.AsString();	
}

BOOL ParameterList::Parameter::getBoolValue() const
{
	return obj.AsBool();
}

int ParameterList::Parameter::getIntValue() const 
{
	return obj.AsInt();
}

vector<int> ParameterList::Parameter::getInts() const
{
	return  obj.AsInts();
}

vector<CString> ParameterList::Parameter::getStrings() const
{
	return obj.AsStrings();
}

float ParameterList::Parameter::getFloatValue() const
{
	return obj.AsFloat();
}

ParameterList::FloatParameter ParameterList::Parameter::getFloats() const
{
	return FloatParameter(interp,obj.AsFloats());
}

Point3 ParameterList::Parameter::getPoint() const
{	
	return obj.AsPoint();
}

Vector3 ParameterList::Parameter::getVector() const
{
	return obj.AsVector();
}

Point2 ParameterList::Parameter::getTexCoord() const
{
	return obj.AsTexCoord();
}

Matrix4 ParameterList::Parameter::getMatrix() const
{
    return obj.AsMatrix();
}

Color ParameterList::Parameter::getColor() const 
{
	return obj.AsColor();
}

ParameterList::Parameter& ParameterList::Parameter::operator=(const Parameter&p)
{
	if(this==&p) return *this;

	t=p.t;
	interp=p.interp;
	obj=p.obj;
	checked=p.checked;

	return *this;
}

ParameterList::ParameterList() 
{	
	numVerts=numFaces=numFaceVerts=0;
}

void ParameterList::clear(BOOL showUnused) 
{
	if(showUnused) 
	{
		hash_map<CString,Parameter,
			hash_compare<const char*,CharLess>>::const_iterator iter;
		for(iter=_list.begin(); iter!=_list.end(); iter++) 
		{
			if(iter->second.checked==NULL)
			    StrUtil::PrintPrompt("未使用参数");
		}
	}
	_list.clear();
	numVerts=numFaces=numFaceVerts=0;
}

BOOL ParameterList::IsNameInList(CString name,Parameter&p) const
{
	hash_map<CString,Parameter,
		hash_compare<const char*,CharLess>>::const_iterator iter;
	iter=_list.find(name);
	if(iter!=_list.end())
	{
		p=iter->second;
        return TRUE;
	}
	
	return FALSE;
}

void ParameterList::setFaceCount(int nFaces) 
{
	numFaces=nFaces;
}

void ParameterList::setVertexCount(int nVerts) 
{
	numVerts=nVerts;
}

void ParameterList::setFaceVertexCount(int nFaceVerts)
{
	numFaceVerts=nFaceVerts;
}

void ParameterList::addString(CString name,CString v) 
{
	add(name,Parameter(v));
}

void ParameterList::addInteger(CString name,int v) 
{
	add(name,Parameter(v));
}

void ParameterList::addBoolean(CString name,bool v) 
{
	add(name,Parameter(v));
}

void ParameterList::addFloat(CString name,float v) 
{
	add(name,Parameter(v));
}

void ParameterList::addColor(CString name,const Color&v) 
{	
	add(name,Parameter(v));
}

void ParameterList::addIntegerArray(CString name,const vector<int>&data)
{
	add(name,Parameter(data));
}

void ParameterList::addStringArray(CString name,const vector<CString>&data) 
{
	add(name,Parameter(data));
}

void ParameterList::addFloats(CString name,InterpolationType interp, 
							  const vector<float>&data) 
{	
	add(name,Parameter(PT_FLOAT,interp,data));
}

void ParameterList::addPoints(CString name,InterpolationType interp,
							  const vector<float>&data) 
{
	add(name,Parameter(PT_POINT,interp,data));
}

void ParameterList::addVectors(CString name,InterpolationType interp, 
							   const vector<float>&data) 
{	
	add(name,Parameter(PT_VECTOR,interp,data));
}

void ParameterList::addTexCoords(CString name,InterpolationType interp,
								 const vector<float>&data) 
{
	add(name,Parameter(PT_TEXCOORD,interp,data));
}

void ParameterList::addMatrices(CString name,InterpolationType interp, 
								const vector<float>&data) 
{
	add(name,Parameter(PT_MATRIX,interp,data));
}

void ParameterList::add(CString name,const Parameter&param) 
{
	if(name.IsEmpty())
		StrUtil::PrintPrompt("空名字不能声明参数");
	_list.insert( CP_PAIR(name,param) );	
}

CString ParameterList::getString(CString name,CString defaultValue)const
{
	Parameter p;
	if(IsNameInList(name,p))	
		if(isValidParameter(name,PT_STRING,IT_NONE,p))
			return p.getStringValue();

	return defaultValue;
}

vector<CString> ParameterList::getStringArray(CString name,
											  const vector<CString>&defaultValue)const
{
	Parameter p;
	if(IsNameInList(name,p))
	    if(isValidParameter(name,PT_STRING,IT_NONE,p))
		    return p.getStrings();

	return defaultValue;
}

int ParameterList::getInt(CString name,int defaultValue)const
{
	Parameter p;
	if(IsNameInList(name,p))
		if(isValidParameter(name,PT_INT,IT_NONE,p))
			return p.getIntValue();

	return defaultValue;
}

vector<int> ParameterList::getIntArray(CString name) const
{
	Parameter p;
	if(IsNameInList(name,p))
		if(isValidParameter(name,PT_INT,IT_NONE,p))
			return p.getInts();

	return vector<int>();
}

BOOL ParameterList::getBoolean(CString name,BOOL defaultValue)const 
{
	Parameter p;
	if(IsNameInList(name,p))
		if(isValidParameter(name,PT_BOOL,IT_NONE,p))
			return p.getBoolValue();

	return defaultValue;
}

float ParameterList::getFloat(CString name,float defaultValue) const
{
	Parameter p;
	if(IsNameInList(name,p))
		if(isValidParameter(name,PT_FLOAT,IT_NONE,p))
			return p.getFloatValue();

	return defaultValue;
}

Color ParameterList::getColor(CString name,const Color&defaultValue) const
{
	Parameter p;
	if(IsNameInList(name,p))
		if(isValidParameter(name,PT_COLOR,IT_NONE,p) )
			return p.getColor();

	return defaultValue;
}

Point3 ParameterList::getPoint(CString name,const Point3&defaultValue) const
{
	Parameter p;
	if(IsNameInList(name,p))
		if(isValidParameter(name,PT_POINT,IT_NONE,p))
			return p.getPoint();

	return defaultValue;
}

Vector3 ParameterList::getVector(CString name,const Vector3&defaultValue)const 
{
	Parameter p;
	if(IsNameInList(name,p))
		if(isValidParameter(name,PT_VECTOR, IT_NONE,p))
			return p.getVector();

	return defaultValue;
}

Point2 ParameterList::getTexCoord(CString name,const Point2&defaultValue)const
{
	Parameter p;
	if(IsNameInList(name,p))
		if(isValidParameter(name,PT_TEXCOORD,IT_NONE,p))
			return p.getTexCoord();

	return defaultValue;
}

Matrix4 ParameterList::getMatrix(CString name,const Matrix4&defaultValue)const
{
	Parameter p;
	if(IsNameInList(name,p))
		if(isValidParameter(name,PT_MATRIX,IT_NONE,p))
			return p.getMatrix();

	return defaultValue;
}

ParameterList::FloatParameter ParameterList::getFloatArray(CString name) const
{
	Parameter p;
	if(IsNameInList(name,p))
	    return getFloatParameter(name,PT_FLOAT,p);
	else return FloatParameter();
}

ParameterList::FloatParameter ParameterList::getPointArray(CString name)const
{
	Parameter p;
	if(IsNameInList(name,p))
		return getFloatParameter(name,PT_POINT,p);
	else return FloatParameter();
}

ParameterList::FloatParameter ParameterList::getVectorArray(CString name)const
{
	Parameter p;
	if(IsNameInList(name,p))
		return getFloatParameter(name,PT_VECTOR,p);
	else return FloatParameter();
}

ParameterList::FloatParameter ParameterList::getTexCoordArray(CString name)const
{
	Parameter p;
	if(IsNameInList(name,p))
		return getFloatParameter(name,PT_TEXCOORD,p);
	else return FloatParameter();
}

ParameterList::FloatParameter ParameterList::getMatrixArray(CString name)const
{
	Parameter p;
	if(IsNameInList(name,p))
		return getFloatParameter(name,PT_MATRIX,p);
	else return FloatParameter();
}

BOOL ParameterList::isValidParameter(CString name,ParameterType t, 
    InterpolationType interp,Parameter&p) const
{	
	if(p.t!=t) 
	{
		StrUtil::PrintPrompt("参数%s申请为%s -声明为%s",
			name,GetParameterTypeName(t), 
			GetParameterTypeName(p.t));
		return FALSE;
	}
	if(p.interp!=interp) 
	{
		StrUtil::PrintPrompt("参数%s申请为%s -声明为%s",
			name,GetInterpolationTypeName(interp), 
			GetInterpolationTypeName(p.interp));
		return FALSE;
	}
	p.checked=TRUE;

	return TRUE;
}

ParameterList::FloatParameter ParameterList::getFloatParameter(
	CString name,ParameterType t,Parameter&p)const
{
	switch(p.interp)
	{
	case IT_NONE:
		if(!isValidParameter(name,t,p.interp,p))
			return FloatParameter();
		break;
	case IT_VERTEX:
		if(!isValidParameter(name,t,p.interp,p))
			return FloatParameter();
		break;
	case IT_FACE:
		if(!isValidParameter(name,t,p.interp,p))
			return FloatParameter();
		break;
	case IT_FACEVARYING:
		if(!isValidParameter(name,t,p.interp,p))
			return FloatParameter();
		break;
	default:
		return FloatParameter();
	}

	return p.getFloats();
}

MovingMatrix4 ParameterList::getMovingMatrix(CString name,MovingMatrix4&defaultValue)const
{	
	Matrix4 m=getMatrix(name,Matrix4());
    if(m!=Matrix4()) return MovingMatrix4(m);

	FloatParameter times=getFloatArray(name+".times");	
	if(times.data.size()<=1)
		defaultValue.updateTimes(0,0);
	else 
	{
		if(times.data.size()!=2)
			StrUtil::PrintPrompt("仅使用%d个指定值的端点时间值",
			times.data.size());
		float t0=times.data[0];
		float t1=times.data[times.data.size()-1];
		defaultValue.updateTimes(t0,t1);
	}

	int steps=getInt(name+".steps",0);
	if(steps<=0) 
	{	
	} 
	else 
	{		
		defaultValue.setSteps(steps);
		for(int i=0; i<steps; i++)
		{   
			CString str;
            str.Format(_T("%s[%d]"),name,i);
			defaultValue.updateData(i,getMatrix(str,defaultValue.getData(i)));
		}
	}

	return defaultValue;
}

ParameterList& ParameterList::operator=(const ParameterList&pl)
{
	if(this==&pl) return *this;

	_list=pl._list;
    numVerts=pl.numVerts;
    numFaces=pl.numFaces;
	numFaceVerts=pl.numFaceVerts;

	return *this;
}