#pragma once

class LGAPIInterface
{
public: 
	virtual ~LGAPIInterface(){};

	virtual void reset();	
	virtual void plugin(CString t,CString name,CString code);	
	virtual void parameter(CString name,CString v);	
	virtual void parameter(CString name,bool v);	
	virtual void parameter(CString name,int v);	
	virtual void parameter(CString name,float v);	
	virtual void parameter(CString name,CString colorSpace,const vector<float>&data);	
	virtual void parameter(CString name,const Point3&v);	
	virtual void parameter(CString name,const Vector3&v);	
	virtual void parameter(CString name,const Point2&v);	
	virtual void parameter(CString name,const Matrix4&v);	
	virtual void parameter(CString name,const vector<int>&v);	
	virtual void parameter(CString name,const vector<CString>&v);	
	virtual void parameter(CString name,CString t, 
		CString interpolation,const vector<float>&data);	
	virtual void remove(CString name);
	virtual void searchPath(CString t,CString path);	
	virtual void shader(CString name,CString shaderType);	
	virtual void modifier(CString name,CString modifierType);	
	virtual void geometry(CString name,CString typeName);	
	virtual void instance(CString name,CString geoname);	
	virtual void light(CString name,CString lightType);	
	virtual void camera(CString name,CString lensType);	
	virtual void options(CString name);	
	virtual void render(CString optionsName,Display*display);	
	virtual BOOL include(CString fileName);	
	virtual void currentFrame(int cf);
};
