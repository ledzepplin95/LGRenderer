#include "StdAfx.h"

#include "RA2Parser.h"
#include "strutil.h"
#include "parser.h"
#include "LGAPI.h"
#include "point3.h"
#include "Vector3.h"

LG_IMPLEMENT_DYNCREATE(RA2Parser,SceneParser)

BOOL RA2Parser::parse(CString fileName,LGAPIInterface&api) 
{
	try
	{
		StrUtil::PrintPrompt("RA2 -读取几何数据: \"%s\" ...",fileName);
		vector<float> data;
        CFile s;
		if( !s.Open(fileName,CFile::modeRead | CFile::typeBinary) )
			return FALSE;
		float dataFloat;
		while(s.Read(&dataFloat,sizeof(dataFloat))<s.GetLength())			
			data.push_back(dataFloat);
		s.Close();		

		api.parameter("points","point","vertex",data);
		vector<int> triangles;
		triangles.resize(3*(data.size()/9));
		for(int i=0; i<triangles.size(); i++)
			triangles[i]=i;
		
		api.parameter("triangles",triangles);
		api.geometry(fileName,"triangle_mesh");		
		api.shader(fileName+".shader","simple");	
		api.parameter("shaders",fileName+".shader");
		api.instance(fileName+".instance",fileName);
	} 
	catch(...) 
	{
		return FALSE;	
	}
	try 
	{
		fileName.Replace(".ra2",".txt");
		StrUtil::PrintPrompt("RA2 -读取相机参数: \"%s\" ...",fileName);
		Parser p(fileName);
		Point3 eye;
		eye.x=p.getNextFloat();
		eye.y=p.getNextFloat();
		eye.z=p.getNextFloat();
		Point3 to;
		to.x=p.getNextFloat();
		to.y=p.getNextFloat();
		to.z=p.getNextFloat();
		Vector3 up;
		switch(p.getNextInt())
		{
		case 0:
			up.set(1,0,0);
			break;
		case 1:
			up.set(0,1,0);
			break;
		case 2:
			up.set(0,0,1);
			break;
		default:
			StrUtil::PrintPrompt("RA2 -指定的up vector无效 - 使用Z轴");
			up.set(0,0,1);
			break;
		}
		api.parameter("eye",eye);
		api.parameter("target",to);
		api.parameter("up",up);
		CString cameraName=fileName+".camera";
		api.parameter("fov",80.0f);
		api.camera(cameraName,"pinhole");
		api.parameter("camera",cameraName);
		api.parameter("resolutionX",1024);
		api.parameter("resolutionY",1024);
		api.options(LGAPI::DEFAULT_OPTIONS);
		p.close();
	} 
	catch(...)
	{	
		return FALSE;
	}

	return TRUE;
}