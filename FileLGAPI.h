#pragma once

#include "lgapiinterface.h"
#include "ParameterList.h"
#include "SCAbstractParser.h"

class FileLGAPI :
	public LGAPIInterface
{
public:
	void close(){};

	void camera(CString name,CString lensType);
    void geometry(CString name,CString typeName);
	int getCurrentFrame();
	void instance(CString name,CString geoname);
	void light(CString name,CString lightType);
	void modifier(CString name,CString modifierType);
	void options(CString name);
	void parameter(CString name,CString v);
	void parameter(CString name,boolean v);
	void parameter(CString name,int v);
	void parameter(CString name,float v);
	void parameter(CString name,CString colorSpace,const vector<float>&data);
	void parameter(CString name,const Point3&v);
	void parameter(CString name,const Vector3&v);
	void parameter(CString name,const Point2&v);
	void parameter(CString name,const Matrix4&v);
	void parameter(CString name,const vector<int>&v);
	void parameter(CString name,const vector<CString>&v);
    void parameter(CString name,CString t,CString interpolation,const vector<float>&data);
	BOOL include(CString fileName);
	void plugin(CString t,CString name,CString code);
	void remove(CString name);
	void render(CString optionsName,Display*display);
	void reset();
	void searchPath(CString t,CString path);
	void currentFrame(int cf);
	void shader(CString name,CString shaderType);

protected:
	FileLGAPI();

	virtual void writeKeyword(SCAbstractParser::SCAP_Keyword k){}

	virtual void writeInterpolationType(ParameterList::InterpolationType interp){}

	virtual void writeBoolean(boolean value){}

	virtual void writeInt(int value){}

	virtual void writeFloat(float value){}

	virtual void writeString(CString string){}

	virtual void writeVerbatimString(CString string){}

	virtual void writeMatrix(const Matrix4&value){}

	virtual void writeNewline(int indentNext){}	

private:
	int frame;

	static ParameterList::InterpolationType GetInterpolationType(const char*interpolation);
};
