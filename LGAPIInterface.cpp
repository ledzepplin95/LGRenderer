#include "stdafx.h"
#include "lgapiinterface.h"

void LGAPIInterface::reset()
{
}
void LGAPIInterface::plugin(CString t,CString name,CString code)
{
}
void LGAPIInterface::parameter(CString name,CString v)
{
}
void LGAPIInterface::parameter(CString name,bool v)
{
}
void LGAPIInterface::parameter(CString name,int v)
{
}
void LGAPIInterface::parameter(CString name,float v)
{
}
void LGAPIInterface::parameter(CString name,CString colorspace,const vector<float>&data)
{
}
void LGAPIInterface::parameter(CString name,const Point3&v)
{
}
void LGAPIInterface::parameter(CString name,const Vector3&v)
{
}
void LGAPIInterface::parameter(CString name,const Point2&v)
{
}
void LGAPIInterface::parameter(CString name,const Matrix4&v)
{
}
void LGAPIInterface::parameter(CString name,const vector<int>&v)
{
}
void LGAPIInterface::parameter(CString name,const vector<CString>&v)
{
}
void LGAPIInterface::parameter(CString name,CString t, 
					   CString interpolation,const vector<float>&data)
{
}
void LGAPIInterface::remove(CString name)
{
}
void LGAPIInterface::searchPath(CString t,CString path)
{
}
void LGAPIInterface::shader(CString name,CString shaderType)
{
}
void LGAPIInterface::modifier(CString name,CString modifierType)
{
}
void LGAPIInterface::geometry(CString name,CString typeName)
{
}
void LGAPIInterface::instance(CString name,CString geoname)
{
}
void LGAPIInterface::light(CString name,CString lightType)
{
}
void LGAPIInterface::camera(CString name,CString lensType)
{
}
void LGAPIInterface::options(CString name)
{
}
void LGAPIInterface::render(CString optionsName,Display*display)
{
}
BOOL LGAPIInterface::include(CString fileName)
{
	return FALSE;
}
void LGAPIInterface::currentFrame(int cf)
{
}