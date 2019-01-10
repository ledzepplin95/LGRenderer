#pragma once

#include "LGAPIInterface.h"
#include "LGSearchPath.h"
#include "RenderObjectMap.h"
#include "ParameterList.h"

class LGAPI :
	public LGAPIInterface
{
public:
	LGAPI();
    ~LGAPI();
	
	void reset();
	void plugin(CString t,CString name,CString code);

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

	void remove(CString name);
	BOOL update(CString name);
	void searchPath(CString t,CString path);
	CString resolveTextureFilename(CString fileName);
	CString resolveIncludeFilename(CString fileName);
	void shader(CString name,CString shaderType);
	void modifier(CString name,CString modifierType);
	void geometry(CString name,CString typeName);
	void instance(CString name,CString geoName);
	void light(CString name,CString lightType);
	void camera(CString name,CString lensType);
	void options(CString name);

	BOOL isIncremental(CString typeName)const;
	Geometry* lookupGeometry(CString name)const;
	Instance* lookupInstance(CString name)const;
	Camera* lookupCamera(CString name)const;
	Options* lookupOptions(CString name)const;
	Shader* lookupShader(CString name)const;
	Modifier* lookupModifier(CString name)const;
	LightSource* lookupLight(CString name)const;
	BoundingBox getBounds()const;	
	int currentFrame()const;

	void render(CString optionsName,Display*display);
	BOOL include(CString fileName);
	void currentFrame(int cf);
	LGAPI& operator=(const LGAPI&api);

	static LGAPI* create(CString fileName,int frameNumber);
	static BOOL translate(CString fileName,CString outputFileName);		

	static CString VERSION;
	static CString DEFAULT_OPTIONS;

private:
	Scene* scene;
	LGSearchPath includeSearchPath;
	LGSearchPath textureSearchPath;
	ParameterList parameterList;
	RenderObjectMap renderObjects;
	int cFrame;
};
