#pragma once

#include "CharLess.h"
#include <hash_map>
#include "object.h"
#include "MovingMatrix4.h"
#include "LGObject.h"

class ParameterList : public LGObject
{
	LG_DECLARE_DYNCREATE(ParameterList);

	enum ParameterType 
	{
		PT_STRING,
		PT_INT,
		PT_BOOL,		
		PT_FLOAT,
		PT_POINT,
		PT_VECTOR,
		PT_TEXCOORD,
		PT_MATRIX,
		PT_COLOR,
	};

public:
	enum InterpolationType
	{
		IT_NONE,
		IT_FACE, 
		IT_VERTEX, 
		IT_FACEVARYING,
	};

    class FloatParameter 
	{
	public:
		FloatParameter();
		FloatParameter(float f);
		FloatParameter(InterpolationType it,
			const vector<float>&d);

		FloatParameter& operator=(const FloatParameter&fp);
		BOOL isNull()const;
	
		InterpolationType interp;
		vector<float> data;
	};

public:

	class Parameter 
	{
	public:
		Parameter();
		Parameter(const Parameter&p);
		Parameter(CString v);
		Parameter(int v);
		Parameter(bool v);
		Parameter(float v);
		Parameter(const vector<int>&a);
		Parameter(const vector<CString>&a);
		Parameter(const Color&c);
		Parameter(ParameterType tt, 
			InterpolationType it,const vector<float>&d);

		int size()const;		
		CString toString()const;
        CString getStringValue()const;
        BOOL getBoolValue()const;
		int getIntValue()const;
		vector<int> getInts()const;
		vector<CString> getStrings()const;
		float getFloatValue()const;
		FloatParameter getFloats()const;
		Point3 getPoint()const;
		Vector3 getVector()const;
		Point2 getTexCoord()const;
		Matrix4 getMatrix()const;
        Color getColor()const;

		void check();

        Parameter& operator=(const Parameter&p);	
	
		ParameterType t;
		InterpolationType interp;
		Object obj;
		BOOL checked;
	}; 

    hash_map<CString,Parameter,hash_compare<const char*,CharLess>> _list;
	typedef pair<CString,Parameter> CP_PAIR;

public:
	ParameterList();
	virtual ~ParameterList(){};

	void clear(BOOL showUnused);
	void setFaceCount(int nFaces);
	void setVertexCount(int nVerts);
	void setFaceVertexCount(int nFaceVerts);
	void addString(CString name,CString v);
	void addInteger(CString name,int v);
	void addBoolean(CString name,bool v);
	void addFloat(CString name,float v);
	void addColor(CString name,const Color&v);
	void addIntegerArray(CString name,const vector<int>&data);
	void addStringArray(CString name,const vector<CString>&data);
	void addFloats(CString name,InterpolationType interp,const vector<float>&data);
	void addPoints(CString name,InterpolationType interp,const vector<float>&data);
	void addVectors(CString name,InterpolationType interp,const vector<float>&data);
	void addTexCoords(CString name,InterpolationType interp,const vector<float>&data);
	void addMatrices(CString name,InterpolationType interp,const vector<float>&data);
	void add(CString name,const Parameter&param);
	ParameterList& operator=(const ParameterList&pl);

	virtual CString getString(CString name,CString defaultValue)const;
    virtual vector<CString> getStringArray(CString name,const vector<CString>&defaultValue)const;
	virtual int getInt(CString name,int defaultValue)const;
	virtual vector<int> getIntArray(CString name)const;
	virtual BOOL getBoolean(CString name,BOOL defaultValue)const;
	virtual float getFloat(CString name,float defaultValue)const;
	virtual Color getColor(CString name,const Color&defaultValue)const;
	virtual Point3 getPoint(CString name,const Point3&defaultValue)const;
	virtual Vector3 getVector(CString name,const Vector3&defaultValue)const;
	virtual Point2 getTexCoord(CString name,const Point2&defaultValue)const;
	virtual Matrix4 getMatrix(CString name,const Matrix4&defaultValue)const;
	virtual FloatParameter getFloatArray(CString name)const;
	virtual FloatParameter getPointArray(CString name)const;
	virtual FloatParameter getVectorArray(CString name)const;
	virtual FloatParameter getTexCoordArray(CString name)const;
	virtual FloatParameter getMatrixArray(CString name)const;
	virtual BOOL isValidParameter(CString name,ParameterType type, 
		InterpolationType interp,Parameter&p)const;
	virtual FloatParameter getFloatParameter(CString name,ParameterType t,Parameter&p)const;
	virtual MovingMatrix4 getMovingMatrix(CString name,MovingMatrix4&defaultValue)const;

private:
	int numVerts,numFaces,numFaceVerts;

	BOOL IsNameInList(CString name,Parameter&p)const;
	static CString GetParameterTypeName(ParameterType ptt);	

public:
	static CString GetInterpolationTypeName(InterpolationType itt);	
	static InterpolationType GetInterpolationType(CString t);
};
